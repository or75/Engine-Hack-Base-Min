#ifndef __CVAR_UTILS_H__
#define __CVAR_UTILS_H__

#include "Main.h"
#include "AutoOffset.h"

extern ofstream logfile;
extern SCREENINFO g_Screen;
extern HINSTANCE hInstance;
extern char BaseDir[MAX_PATH];

typedef void(*PreS_DynamicSound_t)(int, DWORD, char *, float *, DWORD, DWORD, DWORD, DWORD);

typedef void (APIENTRY *glBegin_t)(GLenum);
typedef void (APIENTRY *glEnd_t)();
typedef void (APIENTRY *glVertex3fv_t)(const GLfloat*);
typedef void (APIENTRY *glClear_t)(GLbitfield);

extern PreS_DynamicSound_t PreS_DynamicSound_s;

extern glBegin_t pglBegin;
extern glEnd_t pglEnd;
extern glVertex3fv_t pglVertex3fv;
extern glClear_t pglClear;

extern cl_clientfunc_t *g_pClient;
extern cl_enginefunc_t *g_pEngine;
extern engine_studio_api_t *g_pStudio;

extern cl_clientfunc_t g_Client;
extern cl_enginefunc_t g_Engine;
extern engine_studio_api_t g_Studio;

extern cs_player_info_s PlayerInfo;
extern PUserMsg pUserMsgBase;
extern GameInfo_s BuildInfo;
extern player_s g_Player[33];
extern local_s g_Local;

extern bool InitFont;

#endif