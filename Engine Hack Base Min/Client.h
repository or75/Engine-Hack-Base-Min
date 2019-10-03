#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "Main.h"

void InitHack();
void HUD_Frame(double time);
void HUD_Redraw(float time, int intermission);
void PreS_DynamicSound(int entid, DWORD u, char *szSoundFile, float *fOrigin, DWORD dont, DWORD know, DWORD ja, DWORD ck);

void HookFunction();

#endif