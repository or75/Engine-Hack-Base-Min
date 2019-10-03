#include "Client.h"

void HookUserMessages()
{
#define HOOK_MSG(n) \
	p##n = HookUserMsg(#n, ##n);

	HOOK_MSG(ResetHUD);
	HOOK_MSG(SetFOV);
	HOOK_MSG(TeamInfo);
	HOOK_MSG(CurWeapon);
	HOOK_MSG(DeathMsg);
}

void InitHack()
{
	g_Screen.iSize = sizeof(SCREENINFO);
	g_Engine.pfnGetScreenInfo(&g_Screen);

	g_Engine.Con_Printf("\n\n\t\t\t\tCS 1.6 SIMPLE HOOK BY _OR_75\n\n");

	g_Engine.Con_Printf("\t\t\t\tBuildInfo.GameName: %s\n", BuildInfo.GameName);
	g_Engine.Con_Printf("\t\t\t\tBuildInfo.GameVersion: %s\n", BuildInfo.GameVersion);
	g_Engine.Con_Printf("\t\t\t\tBuildInfo.Protocol: %d\n", BuildInfo.Protocol);
	g_Engine.Con_Printf("\t\t\t\tBuildInfo.Build: %d\n\n", BuildInfo.Build);
}

void HUD_Frame(double time)
{
	static bool FirstFrame = true;
	if (FirstFrame)
	{		
		PlayerInfo = c_Offset.PlayerInfo();
		pUserMsgBase = c_Offset.FindUserMsgBase();

		if (PlayerInfo != 0)
			add_log("PlayerInfo: 0x%X", PlayerInfo);

		add_log("pUserMsgBase: 0x%X", pUserMsgBase);

		c_Offset.GameInfo();		
		c_Offset.Find_SVCBase();		
		c_Offset.Find_MSGInterface();		
		c_Offset.Find_CBuf_AddText();		
		c_Offset.Patch_CL_ConnectionlessPacket();

		CBuf_AddText_Orign = (HL_MSG_CBuf_AddText)c_Offset.pCBuf_AddText;

		//SVC_StuffText_Orign = HookServerMsg(SVC_STUFFTEXT, SVC_StuffText, &c_Offset);
		//SVC_SendCvarValue_Orign = HookServerMsg(SVC_SENDCVARVALUE, SVC_SendCvarValue, &c_Offset);
		//SVC_SendCvarValue2_Orign = HookServerMsg(SVC_SENDCVARVALUE2, SVC_SendCvarValue2, &c_Offset);
		//SVC_Director_Orign = HookServerMsg(SVC_DIRECTOR, SVC_Director, &c_Offset);
		SVC_Sound_Orign = HookServerMsg( SVC_SOUND , SVC_Sound , &c_Offset );

		add_log( "SVC_Sound_Orign: 0x%X" , SVC_Sound_Orign );
		
		InitHack();
		//HookUserMessages();
		HookOpenGL();

		FirstFrame = false;
	}
	return g_Client.HUD_Frame(time);
}

void HUD_Redraw(float time, int intermission)
{
	g_Arial.Print(10, g_Screen.iHeight / 2 - 13, 255, 0, 0, 255, 0, "g_Arial.Print %s", "arg1");
	g_Info.Print( 10 , g_Screen.iHeight / 2 , 0 , 255 , 0 , 255 , 0 , "g_Info.Print %s" , "arg1" );

	g_Info.Print( g_Screen.iWidth / 2 , 10 , 0 , 255 , 0 , 255 , 0 , "[%i/%i/%i]" , (int)g_Local.vOrigin[0] , (int)g_Local.vOrigin[1] , (int)g_Local.vOrigin[2] );

	int lIndex = g_Engine.GetLocalPlayer()->index;
	g_Local.vOrigin = g_Engine.GetLocalPlayer()->origin;
	
	for (int i = 1; i < 33; i++)
	{
		cl_entity_s *ent = g_Engine.GetEntityByIndex(i);
		g_Player[i].vOrigin = ent->origin;
		g_Engine.pfnGetPlayerInfo(i, &g_Player[i].Info);
	}

	//g_Info.Print(g_Screen.iWidth / 2 - 20, 25, 0, 255, 0, 255, 0, "Kills: %i", PlayerInfo[lIndex]->Kills);
	//g_Info.Print(g_Screen.iWidth / 2 - 20, 35, 0, 255, 0, 255, 0, "Deaths: %i", PlayerInfo[lIndex]->Deaths);
	//g_Info.Print(g_Screen.iWidth / 2 - 20, 45, 0, 255, 0, 255, 0, "Health: %i", PlayerInfo[lIndex]->Health);
	//g_Info.Print(g_Screen.iWidth / 2 - 20, 55, 0, 255, 0, 255, 0, "TeamName: %s", PlayerInfo[lIndex]->TeamName);
	//g_Info.Print(g_Screen.iWidth / 2 - 20, 65, 0, 255, 0, 255, 0, "Name: %s", g_Player[lIndex].Info.name);

	g_Drawing.DrawBox(g_Screen.iWidth / 2 - 20, g_Screen.iHeight / 2 - 20, 40, 40, 1, 0, 0, 255, 255);

	return g_Client.HUD_Redraw(time, intermission);
}

void HUD_PlayerMove(struct playermove_s *ppmove, int server)
{
	g_Client.HUD_PlayerMove(ppmove, server);

	g_Engine.pEventAPI->EV_LocalPlayerViewheight(g_Local.vEye);
	g_Local.vEye = g_Local.vEye + ppmove->origin;
}

void PreS_DynamicSound(int entid, DWORD u, char *szSoundFile, float *fOrigin, DWORD dont, DWORD know, DWORD ja, DWORD ck)
{
	__asm nop // убрать если будеш писать здесь код
	PreS_DynamicSound_s(entid, u, szSoundFile, fOrigin, dont, know, ja, ck);
}

void StudioEntityLight(struct alight_s *plight)
{
	g_Engine.Con_Printf("StudioEntityLight\n");
	g_Studio.StudioEntityLight(plight);
}

void HookFunction()
{
	g_pClient->HUD_Frame = HUD_Frame;
	g_pClient->HUD_Redraw = HUD_Redraw;
	g_pClient->HUD_PlayerMove = HUD_PlayerMove;

	//g_pStudio->StudioEntityLight = StudioEntityLight;

	//PreS_DynamicSound_s = (PreS_DynamicSound_t)DetourFunction((LPBYTE)c_Offset.PreS_DynamicSound(), (LPBYTE)&PreS_DynamicSound);
}