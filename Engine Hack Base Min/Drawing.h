#ifndef __DRAWING__
#define __DRAWING__

#include "Main.h"

class CDrawing
{
public:
	void FillArea(int x, int y, int w, int h, int r, int g, int b, int a);
	void glFillRGBA(int x, int y, int w, int h, BYTE r, BYTE g, BYTE b, BYTE a);
	void DrawBox(int x, int y, int w, int h, int linewidth, int r, int g, int b, int a);

	int iStringLen(const char *fmt, ...);
	int iStringHeight(void);

	void DrawString(int x, int y, int r, int g, int b, const char *fmt, ...);
	void DrawStringCenter(int x, int y, int r, int g, int b, const char *fmt, ...);

	void DrawingDrawCircle(int x, int y, int r, bool solid);
	void DrawingDrawLine(int xs, int ys, int xe, int ye);
	void DrawingDrawRectangle(int x, int y, int w, int h, bool solid);
	void DrawingSetLineWidth(float w);
	void DrawingSetColor(int r, int g, int b);
	void DrawingSetColor(int r, int g, int b, int a);
};

extern CDrawing g_Drawing;

#endif