#ifndef __AUTO_OFFSET_H__
#define __AUTO_OFFSET_H__

#include "Main.h"

#define CompareMemory(Buff1, Buff2, Size) __comparemem((const UCHAR *)Buff1, (const UCHAR *)Buff2, (UINT)Size)
#define FindMemoryClone(Start, End, Clone, Size) __findmemoryclone((const ULONG)Start, (const ULONG)End, (const ULONG)Clone, (UINT)Size)
#define FindReference(Start, End, Address) __findreference((const ULONG)Start, (const ULONG)End, (const ULONG)Address)

class AutoOffset
{
private:
	PVOID SVCBase_End;
	UINT SVCCount;

	DWORD GetModuleSize(const DWORD Address);
	DWORD FarProc(const DWORD Address, const DWORD LB, const DWORD HB);

	BOOL __comparemem(const UCHAR *buff1, const UCHAR *buff2, const UINT size);
	ULONG __findmemoryclone(const ULONG start, const ULONG end, const ULONG clone, const UINT size);
	ULONG __findreference(const ULONG start, const ULONG end, const ULONG address);

	DWORD FindPatern(const PCHAR String, const DWORD Base, const DWORD End, const DWORD Offset, bool nullable, bool pointer, bool ret, bool absolute);

	unsigned Relative(const unsigned Addr, const unsigned NewFunc);
	unsigned Absolute(const DWORD Addr);
		
	DWORD GetCallback(const unsigned Index);
	
	bool CheckByte(const DWORD Address, const BYTE Value, const int Offset);

public:
	BYTE HLType;
	DWORD SpeedPtr;
	DWORD pCBuf_AddText;
	server_msg_array_s SVCBase;

	DWORD HwBase, HwSize, HwEnd;
	DWORD ClBase, ClSize, ClEnd;
	DWORD HlBase, HlSize, HlEnd;

	bool GetRendererInfo();
	void Error(const PCHAR Msg);
	
	PVOID SpeedHackPtr();
	PVOID ClientFuncs();
	PVOID EngineFuncs();
	PVOID StudioFuncs();
	cs_player_info_s PlayerInfo();
	void GameInfo();
	void Find_SVCBase();
	void Find_MSGInterface();
	void Find_CBuf_AddText();
	void Patch_CL_ConnectionlessPacket();
	DWORD PreS_DynamicSound();
	PUserMsg FindUserMsgBase();
};

extern AutoOffset c_Offset;

#endif