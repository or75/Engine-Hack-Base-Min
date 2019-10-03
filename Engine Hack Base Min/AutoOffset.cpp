#include "AutoOffset.h"

AutoOffset c_Offset;

#pragma warning(disable:4996)

bool AutoOffset::GetRendererInfo()
{
	DWORD GameUI = (DWORD)GetModuleHandle("GameUI.dll");
	DWORD vgui = (DWORD)GetModuleHandle("vgui.dll");
	DWORD vgui2 = (DWORD)GetModuleHandle("vgui2.dll");
	DWORD d3dim = (DWORD)GetModuleHandle("d3dim.dll");

	HwBase = (DWORD)GetModuleHandle("hw.dll"); // Hardware
	if ( HwBase == NULL )
	{
		HwBase = (DWORD)GetModuleHandle("sw.dll"); // Software
		if ( HwBase == NULL )
		{
			HwBase = (DWORD)GetModuleHandle(NULL); // Non-Steam?
			if ( HwBase == NULL ) // Invalid module handle.
			{
				Error("Invalid module handle.");
			}
			else
				HLType = RENDERTYPE_UNDEFINED;
		}
		else
			HLType = RENDERTYPE_SOFTWARE;
	}
	else
	{
		if (d3dim == NULL)
			HLType = RENDERTYPE_HARDWARE;
		else
			HLType = RENDERTYPE_D3D;
	}

	HwSize = GetModuleSize(HwBase);

	if ( HwSize == NULL )
	{
		switch(HwSize)
		{
		case RENDERTYPE_HARDWARE: HwSize = 0x122A000;break;
		case RENDERTYPE_UNDEFINED: HwSize = 0x2116000;break;
		case RENDERTYPE_SOFTWARE: HwSize = 0xB53000;break;
		default:Error("Invalid renderer type.");
		}
	}

	HwEnd = HwBase + HwSize - 1;

	ClBase = (DWORD)GetModuleHandle("client.dll");

	if (ClBase != NULL) {
		ClSize = GetModuleSize(ClBase);
		ClEnd = ClBase + ClSize - 1;
	}
	else {
		ClBase = HwBase;
		ClEnd = HwEnd;
		ClSize = HwSize;
	}

	HlBase = (DWORD)GetModuleHandle(NULL);
	HlSize = GetModuleSize(HlBase);
	HlEnd = HlBase + HlSize - 1;

	return (HwBase && ClBase && GameUI && vgui && vgui2);
}

void AutoOffset::Error(const PCHAR Msg)
{
	MessageBoxA(0,Msg,"Fatal Error",MB_OK|MB_ICONERROR);
	ExitProcess(0);
}

DWORD AutoOffset::GetModuleSize(const DWORD Address)
{
	return PIMAGE_NT_HEADERS(Address + (DWORD)PIMAGE_DOS_HEADER(Address)->e_lfanew)->OptionalHeader.SizeOfImage;
}

DWORD AutoOffset::FarProc(const DWORD Address, const DWORD LB, const DWORD HB)
{
	return ( (Address < LB) || (Address > HB) );
}

BOOL AutoOffset::__comparemem(const UCHAR *buff1, const UCHAR *buff2, const UINT size)
{
	for (UINT i = 0; i < size; i++, buff1++, buff2++)
	{
		if ((*buff1 != *buff2) && (*buff2 != 0xFF))
			return FALSE;
	}
	return TRUE;
}

ULONG AutoOffset::__findmemoryclone(const ULONG start, const ULONG end, const ULONG clone, const UINT size)
{
	for (ULONG ul = start; (ul + size) < end; ul++)
	{
		if (CompareMemory(ul, clone, size))
			return ul;
	}
	return NULL;
}

ULONG AutoOffset::__findreference(const ULONG start, const ULONG end, const ULONG address)
{
	UCHAR Pattern[5] = { 0 };
	Pattern[0] = 0x68;
	*(ULONG*)&Pattern[1] = address;
	return FindMemoryClone(start, end, Pattern, sizeof(Pattern)-1);
}

bool AutoOffset::CheckByte(const DWORD Address, const BYTE Value, const int Offset)
{
	if (*(PBYTE)((DWORD)Address + Offset) == Value)
		return true;
	else
		return false;
}

DWORD AutoOffset::FindPatern(const PCHAR String, const DWORD Base, const DWORD End, const DWORD Offset, bool nullable = false, bool pointer = true, bool ret = false, bool absolute = false)
{
	char ErrorMsg[256] = { 0 };

	DWORD Address = FindMemoryClone(Base, End, String, (nullable ? strlen(String) + 1: strlen(String)));
	
	if (FarProc(Address, Base, End))
	{
		if (!ret) {
			strcpy_s(ErrorMsg, "Couldn't find ");
			strcat_s(ErrorMsg, String);
			strcat_s(ErrorMsg, " address.");
			Error(ErrorMsg);
		}
		else return 0;
	}
	
	DWORD Reference = (FindReference(Base, End, Address) + Offset);
	DWORD Pattern = (pointer ? (*(PDWORD)Reference) : Reference);
	
	if (FarProc((absolute ? Absolute(Pattern) : Pattern), Base, End))
	{
		if (!ret) {
			strcpy_s(ErrorMsg, "Couldn't find ");
			strcat_s(ErrorMsg, String);
			(pointer ? strcat_s(ErrorMsg, " string pointer.") : strcat_s(ErrorMsg, " string."));
			Error(ErrorMsg);
		}
		else return 0;
	}

	return Pattern;
}

PVOID AutoOffset::SpeedHackPtr()
{
	DWORD Protect = NULL;
	DWORD Address = FindPatern("Texture load: %6.1fms", HwBase, HwEnd, -7, false, true, false, false);
	VirtualProtect((PVOID)Address, sizeof(double), PAGE_READWRITE, &Protect);
	return (PVOID)Address;
}

PVOID AutoOffset::ClientFuncs()
{
	DWORD Address = FindPatern("ScreenFade", HwBase, HwEnd, 0x13, false, true, false, false);
	return (PVOID)Address;
}

PVOID AutoOffset::EngineFuncs()
{
	PVOID EnginePtr = (cl_enginefunc_t*)*(PDWORD)((DWORD)g_pClient->Initialize + 0x22); // old patch
	if (FarProc((DWORD)EnginePtr, HwBase, HwEnd) && FarProc((DWORD)EnginePtr, HlBase, HlEnd)) // fix
	{
		EnginePtr = (cl_enginefunc_t*)*(PDWORD)((DWORD)g_pClient->Initialize + 0x1C); // new patch
		if (FarProc((DWORD)EnginePtr, ClBase, ClEnd))
		{
			EnginePtr = (cl_enginefunc_t*)*(PDWORD)((DWORD)g_pClient->Initialize + 0x1D); // steam
			if (FarProc((DWORD)EnginePtr, ClBase, ClEnd))
			{
				EnginePtr = (cl_enginefunc_t*)*(PDWORD)((DWORD)g_pClient->Initialize + 0x37); // hl-steam
				if (FarProc((DWORD)EnginePtr, ClBase, ClEnd))
				{
					Error("Couldn't find EnginePtr pointer.");
				}
			}
		}
	}
	return EnginePtr;
}

PVOID AutoOffset::StudioFuncs()
{
	PVOID StudioPtr = (engine_studio_api_t*)*(PDWORD)((DWORD)g_pClient->HUD_GetStudioModelInterface + 0x30); // old patch
	if (FarProc((DWORD)StudioPtr, HwBase, HwEnd) && FarProc((DWORD)StudioPtr, HlBase, HlEnd)) // fix
	{
		StudioPtr = (engine_studio_api_t*)*(PDWORD)((DWORD)g_pClient->HUD_GetStudioModelInterface + 0x1A); // new patch / steam	

		if (FarProc((DWORD)StudioPtr, ClBase, ClEnd))
			Error("Couldn't find StudioPtr pointer.");
	}
	return StudioPtr;
}

cs_player_info_s AutoOffset::PlayerInfo()
{
	UINT Size = 0;

	BYTE PlayerInfoString[20] =
	{
		0x0F, 0xBF, 0xC7,
		0x66, 0x8B, 0x4C, 0x24, 0x18,
		0x8D, 0x14, 0x40,
		0x8D, 0x04, 0x90,
		0xC1, 0xE0, 0x03,
		0x66, 0x89, 0x98
	};

	BYTE PlayerInfoStringOld[28] = 
	{
		0x0F, 0xBF, 0xC7,
		0x66, 0x8B, 0x4C, 0x24, 0x18,
		0x8D, 0x14, 0x40,
		0x8D, 0x04, 0x90,
		0xC1, 0xE0, 0x03,
		0x66, 0x83, 0xB8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
		0x66, 0x89, 0x98
	};

	DWORD PlayerInfo = FindMemoryClone(ClBase, ClEnd, &PlayerInfoString, sizeof(PlayerInfoString));
	Size = 20;

	if (FarProc(PlayerInfo, ClBase, ClEnd))
	{
		Size = 28;
		PlayerInfo = FindMemoryClone(ClBase, ClEnd, &PlayerInfoStringOld, sizeof(PlayerInfoStringOld));

		if (FarProc(PlayerInfo, ClBase, ClEnd))
			return 0;

		return cs_player_info_s(UINT(*PUINT(UINT(PlayerInfo) + Size)));
	}
	
	return cs_player_info_s(UINT(*PUINT(UINT(PlayerInfo) + Size)));
}

unsigned AutoOffset::Relative(const UINT Addr, const UINT NewFunc)
{
	return NewFunc - Addr - 5;
}

unsigned AutoOffset::Absolute(const DWORD Address)
{
	return UINT(Address) + *(PUINT)(Address)+4;
}

DWORD AutoOffset::GetCallback(const unsigned Index)
{
	return SVCBase[Index]->Callback;
}

void AutoOffset::GameInfo()
{
	pcmd_t cmd = CommandByName("version");
	DWORD Address = cmd->pfnFunc;

	BuildInfo.GameName = *(PCHAR*)(UINT(Address) + 1);
	BuildInfo.GameVersion = *(PCHAR*)(UINT(Address) + 6);
	BuildInfo.Protocol = *(PBYTE)(UINT(Address) + 11);
	Address = Absolute(UINT(Address) + 23);

	if (FarProc(Address, HwBase, HwEnd))
		Error("Couldn't find GameInfo pointer.");

	__asm // ну куда в c++ без asm ? D:
	{
		call Address // а теперь вызываем функцию которо€ возвращ€ет build
		mov BuildInfo.Build, eax // ну и return себе пихаем
	}
}

void AutoOffset::Find_SVCBase()
{
	DWORD Address = FindPatern("-------- Message Load ---------", HwBase, HwEnd, 0, false, false, false, false);
	
	if (!CheckByte(Address, 0xBF, 0x11))
		Error("Couldn't find SVCBase #1 pointer.");
	else
		Address = Address + 0x12;

	PVOID Addr = (PVOID)Address;
	DWORD SvPtr = Address;
	Addr = *(PUINT*)(Addr);

	SVCBase = (server_msg_array_s)PVOID(UINT(Addr) - sizeof(UINT));

	if (!CheckByte(SvPtr, 0x81, 0x9) && !CheckByte(SvPtr, 0xFF, 0x10))
		Error("Couldn't find SVCBase #2 pointer.");
	else
		SvPtr = SvPtr + 0x11;

	Addr = (PVOID)SvPtr;
	SVCBase_End = PVOID(*PUINT(Addr) + sizeof(UINT));
	SVCCount = (UINT(SVCBase_End) - UINT(SVCBase)) / sizeof(server_msg_t);
}

void AutoOffset::Find_MSGInterface()
{
	MSG_ReadByte = (HL_MSG_ReadByte)(DWORD)(PVOID)PVOID( Absolute( GetCallback( SVC_CDTRACK ) + 1 ) );
	MSG_ReadShort = (HL_MSG_ReadShort)(DWORD)(PVOID)PVOID( Absolute( GetCallback( SVC_STOPSOUND ) + 1 ) );
	MSG_ReadLong = (HL_MSG_ReadLong)(DWORD)(PVOID)PVOID( Absolute( GetCallback( SVC_VERSION ) + 1 ) );
	MSG_ReadFloat = (HL_MSG_ReadFloat)(DWORD)(PVOID)PVOID( Absolute( GetCallback( SVC_TIMESCALE ) + 1 ) );
	MSG_ReadString = (HL_MSG_ReadString)(DWORD)(PVOID)PVOID( Absolute( GetCallback( SVC_PRINT ) + 1 ) );

	MSG_ReadCount = *(PINT*)(UINT(MSG_ReadByte) + 1);
	MSG_CurrentSize = *(PINT*)(UINT(MSG_ReadByte) + 7);
	MSG_BadRead = *(PINT*)(UINT(MSG_ReadByte) + 20);
}

void AutoOffset::Find_CBuf_AddText()
{
	PVOID Address;
	pcmd_t cmd = CommandByName("escape");

	Address = PVOID(UINT(cmd->pfnFunc) + 15);
	pCBuf_AddText = (DWORD)PVOID(Absolute((DWORD)Address));

	if (FarProc((DWORD)Address, HwBase, HwEnd))
		Error("Couldn't find CBuf_AddText #1 pointer.");

	if (FarProc((DWORD)pCBuf_AddText, HwBase, HwEnd))
		Error("Couldn't find CBuf_AddText #2 pointer.");
}

void AutoOffset::Patch_CL_ConnectionlessPacket()
{
	DWORD Protect = NULL;
	DWORD Address = FindPatern("Redirecting connection to", HwBase, HwEnd, 0, false, false, false, false);

	if (!CheckByte(Address, 0xE8, -6))
		Error("Couldn't find CL_ConnectionlessPacket pointer.");
	else
		Address = Address - 5;

	VirtualProtect((PVOID)Address, 4, PAGE_EXECUTE_READWRITE, &Protect);
	*PUINT(Address) = Relative(UINT(Address) - 1, UINT(&CBuf_AddText));
	VirtualProtect((PVOID)Address, 4, Protect, NULL);
}

DWORD AutoOffset::PreS_DynamicSound()
{
	DWORD Address = FindPatern("CL_Parse_Sound: ent = %i, cl.max_edicts %i", HwBase, HwEnd, -0x10, false, false, true, true);
	
	if (!Address)
		Address = FindPatern("CL_Parse_Sound: ent = %i, cl.max_edicts %i", HwBase, HwEnd, -0x11, false, false, false, true);

	return Absolute( Address );
}

PUserMsg AutoOffset::FindUserMsgBase()
{
	BYTE Pattern_UserMsg[8] = 
	{
		0x52, 0x50, 0xE8, 0xFF, 0xFF, 0xFF, 0xFF, 0x83
	};

	BYTE Pattern_UserMsg2[12] = 
	{
		0xFF, 0xFF, 0xFF, 0x0C,
		0x56, 0x8B, 0x35, 0xFF, 0xFF, 0xFF, 0xFF, 0x57
	};

	UINT Address = Absolute((UINT)FindMemoryClone((DWORD)g_Engine.pfnHookUserMsg, (DWORD)g_Engine.pfnHookUserMsg + 0x32,
		&Pattern_UserMsg, sizeof(Pattern_UserMsg)) + 3);

	if (FarProc(Address, HwBase, HwEnd))
		Error("Couldn't find UserMsgBase #1 pointer.");
	
	Address = (UINT)FindMemoryClone((DWORD)Address, (DWORD)Address + 0x32,
		&Pattern_UserMsg2, sizeof(Pattern_UserMsg2)) + 7;

	if (FarProc(Address, HwBase, HwEnd))
		Error("Couldn't find UserMsgBase #2 pointer.");

	return PUserMsg(**(PUINT*)(Address));
}