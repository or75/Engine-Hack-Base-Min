#include "Main.h"

ofstream logfile;
SCREENINFO g_Screen;
HINSTANCE hInstance;
char BaseDir[MAX_PATH];

PreS_DynamicSound_t PreS_DynamicSound_s = nullptr;

glBegin_t pglBegin = nullptr;
glEnd_t pglEnd = nullptr;
glVertex3fv_t pglVertex3fv = nullptr;
glClear_t pglClear = nullptr;

cl_clientfunc_t *g_pClient = nullptr;
cl_enginefunc_t *g_pEngine = nullptr;
engine_studio_api_t *g_pStudio = nullptr;

cl_clientfunc_t g_Client;
cl_enginefunc_t g_Engine;
engine_studio_api_t g_Studio;

cs_player_info_s PlayerInfo = nullptr;
PUserMsg pUserMsgBase = nullptr;

GameInfo_s BuildInfo;
player_s g_Player[33];
local_s g_Local;

bool InitFont = false;