#include "Main.h"

typedef struct _PEB_LDR_DATA {
	UINT8 _PADDING_[12];
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _PEB {
	UINT8 _PADDING_[12];
	PEB_LDR_DATA* Ldr;
} PEB, *PPEB;

typedef struct _LDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	VOID* DllBase;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef struct _UNLINKED_MODULE
{
	HMODULE hModule;
	PLIST_ENTRY RealInLoadOrderLinks;
	PLIST_ENTRY RealInMemoryOrderLinks;
	PLIST_ENTRY RealInInitializationOrderLinks;
	PLDR_DATA_TABLE_ENTRY Entry;
} UNLINKED_MODULE;

#define UNLINK(x)               \
	(x).Flink->Blink = (x).Blink;   \
	(x).Blink->Flink = (x).Flink;

#define RELINK(x, real)   \
	(x).Flink->Blink = (real);  \
	(x).Blink->Flink = (real);  \
	(real)->Blink = (x).Blink;  \
	(real)->Flink = (x).Flink;

std::vector<UNLINKED_MODULE> UnlinkedModules;

struct FindModuleHandle
{
	HMODULE m_hModule;
	FindModuleHandle(HMODULE hModule) : m_hModule(hModule)
	{
	}
	bool operator() (UNLINKED_MODULE const &Module) const
	{
		return (Module.hModule == m_hModule);
	}
};

void RelinkModuleToPEB(HMODULE hModule)
{
	std::vector<UNLINKED_MODULE>::iterator it = std::find_if(UnlinkedModules.begin(), UnlinkedModules.end(), FindModuleHandle(hModule));

	if (it == UnlinkedModules.end())
	{
		return;
	}

	RELINK((*it).Entry->InLoadOrderLinks, (*it).RealInLoadOrderLinks);
	RELINK((*it).Entry->InInitializationOrderLinks, (*it).RealInInitializationOrderLinks);
	RELINK((*it).Entry->InMemoryOrderLinks, (*it).RealInMemoryOrderLinks);
	UnlinkedModules.erase(it);
}

void UnlinkModuleFromPEB(HMODULE hModule)
{
	std::vector<UNLINKED_MODULE>::iterator it = std::find_if(UnlinkedModules.begin(), UnlinkedModules.end(), FindModuleHandle(hModule));
	if (it != UnlinkedModules.end())
	{
		return;
	}

#ifdef _WIN64
	PPEB pPEB = (PPEB)__readgsqword(0x60);
#else
	PPEB pPEB = (PPEB)__readfsdword(0x30);
#endif

	PLIST_ENTRY CurrentEntry = pPEB->Ldr->InLoadOrderModuleList.Flink;
	PLDR_DATA_TABLE_ENTRY Current = NULL;

	while (CurrentEntry != &pPEB->Ldr->InLoadOrderModuleList && CurrentEntry != NULL)
	{
		Current = CONTAINING_RECORD(CurrentEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
		if (Current->DllBase == hModule)
		{
			UNLINKED_MODULE CurrentModule = { 0 };
			CurrentModule.hModule = hModule;
			CurrentModule.RealInLoadOrderLinks = Current->InLoadOrderLinks.Blink->Flink;
			CurrentModule.RealInInitializationOrderLinks = Current->InInitializationOrderLinks.Blink->Flink;
			CurrentModule.RealInMemoryOrderLinks = Current->InMemoryOrderLinks.Blink->Flink;
			CurrentModule.Entry = Current;
			UnlinkedModules.push_back(CurrentModule);

			UNLINK(Current->InLoadOrderLinks);
			UNLINK(Current->InInitializationOrderLinks);
			UNLINK(Current->InMemoryOrderLinks);

			break;
		}
		CurrentEntry = CurrentEntry->Flink;
	}
}

void RemoveHeader(HINSTANCE hinstDLL)
{
	DWORD ERSize = 0;
	DWORD Protect = 0;
	DWORD dwStartOffset = (DWORD)hinstDLL;

	IMAGE_DOS_HEADER *pDosHeader = (PIMAGE_DOS_HEADER)dwStartOffset;
	IMAGE_NT_HEADERS *pNtHeader = (PIMAGE_NT_HEADERS)(dwStartOffset + pDosHeader->e_lfanew);

	ERSize = sizeof(IMAGE_NT_HEADERS);

	if (VirtualProtect(pDosHeader, ERSize, PAGE_EXECUTE_READWRITE, &Protect)) {
		for (DWORD i = 0; i < ERSize - 1; i++)
			*(PBYTE)((DWORD)pDosHeader + i) = 0x00;
	}

	VirtualProtect(pDosHeader, ERSize, Protect, 0);

	ERSize = sizeof(IMAGE_DOS_HEADER);

	if ((pNtHeader != 0) && VirtualProtect(pNtHeader, ERSize, PAGE_EXECUTE_READWRITE, &Protect)) {
		for (DWORD i = 0; i < ERSize - 1; i++)
			*(PBYTE)((DWORD)pNtHeader + i) = 0x00;
	}

	VirtualProtect(pNtHeader, ERSize, Protect, 0);
}

void HideModuleXta(HINSTANCE hModule)
{
	DWORD dwPEB_LDR_DATA = 0;
	_asm
	{
		pushad;
		pushfd;
		mov eax, fs:[30h]
		mov eax, [eax + 0Ch]
		mov dwPEB_LDR_DATA, eax
		mov esi, [eax + 0Ch]
		mov edx, [eax + 10h]
		LoopInLoadOrderModuleList :
		lodsd
		mov esi, eax
		mov ecx, [eax + 18h]
		cmp ecx, hModule
		jne SkipA
		mov ebx, [eax]
		mov ecx, [eax + 4]
		mov[ecx], ebx
		mov[ebx + 4], ecx
		jmp InMemoryOrderModuleList
		SkipA :
		cmp edx, esi
		jne LoopInLoadOrderModuleList
		InMemoryOrderModuleList :
		mov eax, dwPEB_LDR_DATA
		mov esi, [eax + 14h]
		mov edx, [eax + 18h]
		LoopInMemoryOrderModuleList :
		lodsd
		mov esi, eax
		mov ecx, [eax + 10h]
		cmp ecx, hModule
		jne SkipB
		mov ebx, [eax]
		mov ecx, [eax + 4]
		mov[ecx], ebx
		mov[ebx + 4], ecx
		jmp InInitializationOrderModuleList
		SkipB :
		cmp edx, esi
		jne LoopInMemoryOrderModuleList
		InInitializationOrderModuleList :
		mov eax, dwPEB_LDR_DATA
		mov esi, [eax + 1Ch]
		mov edx, [eax + 20h]
		LoopInInitializationOrderModuleList:
		lodsd
		mov esi, eax
		mov ecx, [eax + 08h]
		cmp ecx, hModule
		jne SkipC
		mov ebx, [eax]
		mov ecx, [eax + 4]
		mov[ecx], ebx
		mov[ebx + 4], ecx
		jmp Finished
		SkipC:
		cmp edx, esi
		jne LoopInInitializationOrderModuleList
		Finished:
		popfd
		popad
	}
}

void InitializeSecuritySystem()
{
	HideModuleXta(hInstance);
	UnlinkModuleFromPEB(hInstance);
	RemoveHeader(hInstance);
}