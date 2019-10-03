#include "Main.h"

#pragma warning(disable:4996)

void CDrawing::DrawingSetColor(int r, int g, int b)
{
	glColor3ub(r, g, b);
}

void CDrawing::DrawingSetColor(int r, int g, int b, int a)
{
	glColor4ub(r, g, b, a);
}

void CDrawing::DrawingSetLineWidth(float w)
{
	glLineWidth(1.0f);
}

void CDrawing::DrawingDrawRectangle(int x, int y, int w, int h, bool solid)
{
	if (!solid)
		glBegin(GL_LINE_LOOP);
	else
		glBegin(GL_QUADS);

	glVertex2i(x - w / 2, y - h / 2);
	glVertex2i(x + w / 2 + 1, y - h / 2);
	glVertex2i(x + w / 2, y + h / 2);
	glVertex2i(x - w / 2, y + h / 2);

	glEnd();
}

void CDrawing::DrawingDrawLine(int xs, int ys, int xe, int ye)
{
	glBegin(GL_LINES);
	glVertex2i(xs, ys);
	glVertex2i(xe, ye);
	glEnd();
}

void CDrawing::DrawingDrawCircle(int x, int y, int r, bool solid)
{
	float vectorX, vectorY1 = (float)y + r;
	float vectorY, vectorX1 = (float)x;
	glBegin(GL_LINE_STRIP);
	for (float angle = 0.0f; angle <= (2.0f*3.14159); angle += 0.01f)
	{
		vectorX = (float)x + (float)r * sin(angle);
		vectorY = (float)y + (float)r * cos(angle);
		glVertex2f(vectorX1, vectorY1);
		vectorY1 = vectorY;
		vectorX1 = vectorX;
	}
	glEnd();
}

void CDrawing::FillArea(int x, int y, int w, int h, int r, int g, int b, int a)
{
	g_Engine.pfnFillRGBA(x, y, w, h, r, g, b, a);
}

void CDrawing::DrawBox(int x, int y, int w, int h, int linewidth, int r, int g, int b, int a)
{
	//        X						Y					W					H				R  G  B  A
	FillArea(x, y, w, linewidth, r, g, b, a); // top
	FillArea(x + w - linewidth, y + linewidth, linewidth, h - linewidth, r, g, b, a); // right
	FillArea(x, y + linewidth, linewidth, h - linewidth, r, g, b, a); // left
	FillArea(x + linewidth, y + h - linewidth, w - linewidth * 2, linewidth, r, g, b, a); // bottom
}

int CDrawing::iStringLen(const char *fmt, ...)
{
	va_list va_alist;
	char buf[256];
	va_start(va_alist, fmt);
	_vsnprintf(buf, sizeof(buf), fmt, va_alist);
	va_end(va_alist);
	int iWidth, iHeight;
	g_Engine.pfnDrawConsoleStringLen(buf, &iWidth, &iHeight);
	return iWidth;
}

int CDrawing::iStringHeight(void)
{
	int iWidth, iHeight;
	g_Engine.pfnDrawConsoleStringLen("F", &iWidth, &iHeight);
	return iHeight;
}

void CDrawing::DrawString(int x, int y, int r, int g, int b, const char *fmt, ...)
{
	va_list va_alist;
	char buf[256];
	va_start(va_alist, fmt);
	_vsnprintf(buf, sizeof(buf), fmt, va_alist);
	va_end(va_alist);
	g_Engine.pfnDrawSetTextColor((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f);
	g_Engine.pfnDrawConsoleString(x, y, buf);
}

void CDrawing::DrawStringCenter(int x, int y, int r, int g, int b, const char *fmt, ...)
{
	va_list va_alist;
	char buf[256];
	va_start(va_alist, fmt);
	_vsnprintf(buf, sizeof(buf), fmt, va_alist);
	va_end(va_alist);
	int iWidth = iStringLen("%s", buf);
	g_Engine.pfnDrawSetTextColor((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f);
	g_Engine.pfnDrawConsoleString(x - iWidth / 2, y, buf);
}

void CDrawing::glFillRGBA(int x, int y, int w, int h, BYTE r, BYTE g, BYTE b, BYTE a)
{
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4ub(r, g, b, a);
	glBegin(GL_QUADS);
	glVertex2i(x, y);
	glVertex2i(x + w, y);
	glVertex2i(x + w, y + h);
	glVertex2i(x, y + h);
	glEnd();
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
}

CDrawing g_Drawing;