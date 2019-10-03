#include "UserMsgs.h"

pfnUserMsgHook pResetHUD;
pfnUserMsgHook pSetFOV;
pfnUserMsgHook pTeamInfo;
pfnUserMsgHook pCurWeapon;
pfnUserMsgHook pDeathMsg;

int ResetHUD(const char *pszName, int iSize, void *pbuf)
{
	add_log("ResetHUD");
	return pResetHUD(pszName, iSize, pbuf);
}

int SetFOV(const char *pszName, int iSize, void *pbuf)
{
	add_log("SetFOV");
	return pSetFOV( pszName, iSize, pbuf );
}

int TeamInfo(const char *pszName, int iSize, void *pbuf)
{
	add_log("TeamInfo");
	return pTeamInfo(pszName, iSize, pbuf);
}

int CurWeapon(const char *pszName, int iSize, void *pbuf)
{
	add_log("CurWeapon");
	return pCurWeapon(pszName, iSize, pbuf);
}

int DeathMsg(const char *pszName, int iSize, void *pbuf)
{
	add_log("DeathMsg");
	return pDeathMsg(pszName, iSize, pbuf);
}