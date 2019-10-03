#include "Main.h"

void APIENTRY Hooked_glBegin(GLenum mode)
{
	if (!InitFont)
	{
		g_Info.InitText();
		g_Arial.InitText();
		InitFont = true;
	}
	pglBegin(mode);
}

void APIENTRY Hooked_glEnd()
{
	pglEnd();
}

void APIENTRY Hooked_glVertex3fv(GLfloat *v)
{
	pglVertex3fv(v);
}

void APIENTRY Hooked_glClear(GLbitfield mask)
{
	pglClear(mask);
}

PVOID DetourFunc(BYTE *src, const BYTE *dst, const int len)
{
	BYTE *jmp = (BYTE*)malloc(len + 5);
	DWORD dwback;
	VirtualProtect(src, len, PAGE_READWRITE, &dwback);
	memcpy(jmp, src, len);
	jmp += len;
	jmp[0] = 0xE9;
	*(DWORD*)(jmp + 1) = (DWORD)(src + len - jmp) - 5;
	src[0] = 0xE9;
	*(DWORD*)(src + 1) = (DWORD)(dst - src) - 5;
	VirtualProtect(src, len, dwback, &dwback);
	return (jmp - len);
}

void HookOpenGL()
{
	HMODULE hmOpenGL = GetModuleHandle("opengl32.dll");
	
	pglBegin = (glBegin_t)DetourFunc((LPBYTE)GetProcAddress( hmOpenGL, "glBegin" ), (LPBYTE)&Hooked_glBegin, 6);
	//pglEnd = (glEnd_t)DetourFunc((LPBYTE)GetProcAddress( hmOpenGL, "glEnd" ), (LPBYTE)&Hooked_glEnd, 6);
	//pglVertex3fv = (glVertex3fv_t)DetourFunc((LPBYTE)GetProcAddress( hmOpenGL, "glVertex3fv" ), (LPBYTE)&Hooked_glVertex3fv, 6);
	//pglClear = (glClear_t)DetourFunc((LPBYTE)GetProcAddress( hmOpenGL, "glClear" ), (LPBYTE)&Hooked_glClear, 7);
}