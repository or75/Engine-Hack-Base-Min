#include "Main.h"

#pragma warning(disable:4996)

void add_log(const char *fmt, ...)
{
	if (!fmt)
		return;
	va_list va_alist;
	char logbuf[256] = { 0 };
	va_start(va_alist, fmt);
	_vsnprintf(logbuf + strlen(logbuf), sizeof(logbuf)-strlen(logbuf), fmt, va_alist);
	va_end(va_alist);
	logfile << logbuf << endl;
}

void SetupHook()
{
	while (!c_Offset.GetRendererInfo())
		Sleep(100);

	g_pClient = (cl_clientfunc_t*)c_Offset.ClientFuncs();
	g_pEngine = (cl_enginefunc_t*)c_Offset.EngineFuncs();
	g_pStudio = (engine_studio_api_t*)c_Offset.StudioFuncs();
		
	add_log("g_pClient: 0x%X", g_pClient);
	add_log("g_pEngine: 0x%X", g_pEngine);
	add_log("g_pStudio: 0x%X", g_pStudio);
	add_log("PreS_DynamicSound: 0x%X", c_Offset.PreS_DynamicSound());
		
	Hook:

	RtlCopyMemory(&g_Client, g_pClient, sizeof(cl_clientfunc_t));
	RtlCopyMemory(&g_Engine, g_pEngine, sizeof(cl_enginefunc_t));	
	RtlCopyMemory(&g_Studio, g_pStudio, sizeof(engine_studio_api_t));
	
	if (!g_Client.V_CalcRefdef || !g_Engine.V_CalcShake || !g_Studio.StudioSetupSkin)
		goto Hook;

	HookFunction();
}

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
		if (dwReason == DLL_PROCESS_ATTACH)
		{
			if (GetLastError() != ERROR_ALREADY_EXISTS)
			{
				hInstance = hModule;
				DisableThreadLibraryCalls((HINSTANCE)hModule);

				GetModuleFileNameA(hModule, BaseDir, sizeof(BaseDir));
				char* pos = BaseDir + strlen(BaseDir);
				while (pos >= BaseDir && *pos != '\\') --pos; pos[1] = 0;

				char LogFile[MAX_PATH];
				strcpy_s(LogFile, BaseDir);
				strcat_s(LogFile, "\\offset.txt");
				remove(LogFile);
				logfile.open(LogFile, ios::app);

				InitializeSecuritySystem();
				CreateThread(0, 0, (LPTHREAD_START_ROUTINE)SetupHook, 0, 0, 0);
			}	
		}
		break;
	}
	return TRUE;
}