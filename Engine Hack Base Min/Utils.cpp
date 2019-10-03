#include "Utils.h"

#pragma warning(disable:4996)

pcmd_t CommandByName(char* szName)
{	
	pcmd_t pCmd = nullptr;
	pCmd = g_Engine.pfnGetCmdList();
	while( pCmd )
	{
		if( !strcmp( pCmd->pszName, szName) )
			return pCmd;
		pCmd = pCmd->pNext;
	}
	return 0;
}

PUserMsg UserMsgByName(char* szMsgName)
{
	PUserMsg Ptr = nullptr;
	Ptr = pUserMsgBase;
	while (Ptr->next)
	{
		if (!strcmp(Ptr->name, szMsgName))
			return Ptr;
		Ptr = Ptr->next;
	}
	Ptr->pfn = 0;
	return Ptr;
}

pfnUserMsgHook HookUserMsg(char *szMsgName, pfnUserMsgHook pfn)
{
	PUserMsg Ptr = nullptr;
	pfnUserMsgHook Original = nullptr;
	Ptr = UserMsgByName(szMsgName);
	if (Ptr->pfn != 0) {
		Original = Ptr->pfn;
		Ptr->pfn = pfn;
		return Original;
	}
	else
	{
		char Message[256];
		strcpy(Message, "ERROR: Couldn't find ");
		strcat(Message, szMsgName);
		strcat(Message, " message.");
		c_Offset.Error(Message);
	}
	return 0;
}