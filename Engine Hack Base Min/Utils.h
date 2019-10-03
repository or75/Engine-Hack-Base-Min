#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "Main.h"

pcmd_t CommandByName(char* szName);
PUserMsg UserMsgByName(char* szMsgName);
pfnUserMsgHook HookUserMsg(char *szMsgName, pfnUserMsgHook pfn);

#endif