#include "stdafx.h"
#include <gl\GL.h>
#include "gl/glext.h"
#include "View.h"
#include <cstring>

#pragma comment(lib, "freetyped.lib")

LRESULT CView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	PIXELFORMATDESCRIPTOR pfd;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);//������ʽ�������Ĵ�С
	pfd.nVersion = 1;//�汾��
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	//��ʽ֧�֣����ڡ�OpenGL��˫����
	pfd.iPixelType = PFD_TYPE_RGBA;//����RGBA��ʽ
	pfd.cColorBits = 32;//ѡ��ɫ�����
	pfd.cRedBits = 0;//����ɫ��λ
	pfd.cRedShift = 0;
	pfd.cGreenBits = 0;
	pfd.cGreenShift = 0;
	pfd.cBlueBits = 0;
	pfd.cBlueShift = 0;
	pfd.cAlphaBits = 0;//wuAlpha����
	pfd.cAlphaShift = 0;//����Shift Bit
	pfd.cAccumBits = 0;//�޾ۼ�����
	pfd.cAccumRedBits = 0;//���Ծۼ�λ
	pfd.cAccumGreenBits = 0;
	pfd.cAccumBlueBits = 0;
	pfd.cAccumAlphaBits = 0;
	pfd.cDepthBits = 16;//16λ ��Ȼ��棨z-���棩
	pfd.cStencilBits = 0;//��ģ�建��
	pfd.cAuxBuffers = 0;//�޸�������
	pfd.iLayerType = PFD_MAIN_PLANE;//����ͼ��
	pfd.bReserved = 0;//����
	pfd.dwLayerMask = 0;//�������ֲ�
	pfd.dwVisibleMask = 0;
	pfd.dwDamageMask = 0;

	int iPixelFormat = 0;

	_hDC = GetDC();
	if (!(iPixelFormat = ChoosePixelFormat(_hDC, &pfd)))
	{
		MessageBox(_T("Can't Find A Suitable PixelFormat."), _T("err"), MB_OK | MB_ICONERROR);
		return false;
	}

	if (!SetPixelFormat(GetDC(), iPixelFormat, &pfd))
	{
		MessageBox(_T("Can't Find A Suitable PixelFormat."), _T("err"), MB_OK | MB_ICONERROR);
	}
	_hGLRC = wglCreateContext(_hDC);
	wglMakeCurrent(_hDC, _hGLRC);

	glShadeModel(GL_SMOOTH);                 // ������Ӱƽ��  
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);    // ��ɫ����  
	glClearDepth(1.0f);                      // ������Ȼ���  
	glEnable(GL_DEPTH_TEST);                 // ������Ȳ���  
	glDepthFunc(GL_LEQUAL);                  // ������Ȳ��Ե�����  
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);// ����ϵͳ��͸�ӽ�������  

	_tick = 0;

	//init freetype
	if (FT_Init_FreeType(&_ftLib))
	{
		return false;
	}

	if (FT_New_Face(_ftLib, "FreeSans.ttf", 0, &_ftFace))
	{
		return false;
	}

	FT_Set_Pixel_Sizes(_ftFace, 0, 24);

	if (FT_Load_Char(_ftFace, 'X', FT_LOAD_RENDER))
	{
		return false;
	}

	
	return true;
}


void CView::render_text(const char *text, float x, float y, float sx, float sy) {
	const char *p;
	for (p = text; *p; p++) {
		if (FT_Load_Char(_ftFace, *p, FT_LOAD_RENDER))
			continue;
		FT_GlyphSlot g = _ftFace->glyph;
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			g->bitmap.width,
			g->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			g->bitmap.buffer
		);

		float x2 = x + g->bitmap_left * sx;
		float y2 = -y - g->bitmap_top * sy;
		float w = g->bitmap.width * sx;
		float h = g->bitmap.rows * sy;

		GLfloat box[4][4] = {
			{x2,     -y2    , 0, 0},
			{x2 + w, -y2    , 1, 0},
			{x2,     -y2 - h, 0, 1},
			{x2 + w, -y2 - h, 1, 1},
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		x += (g->advance.x / 64) * sx;
		y += (g->advance.y / 64) * sy;
	}
}

LRESULT CView::OnDestroy(UINT, WPARAM, LPARAM, BOOL &)
{
	wglMakeCurrent(nullptr, nullptr);
	wglDeleteContext(_hGLRC);
	ReleaseDC(_hDC);
	PostQuitMessage(0);
	return true;
}

LRESULT CView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	PAINTSTRUCT ps;
	BeginPaint(&ps);
	EndPaint(&ps);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(0, 1, 0);

	glBegin(GL_LINE_STRIP);
	for (float xx = -1.0; xx <= 1.0; xx += 0.05f)
	{
		glVertex3d(xx, 0.5 * sinf(2 * 3.14f * xx + _tick), 0);
	}
	glEnd();

	render_text("ben", 0, 0, 0.1, 0.1);

	glFlush();
	SwapBuffers(_hDC);
	
	return 0;
}

LRESULT CView::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
	return 0;
}

LRESULT CView::OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{

	return 0;
}