#include "stdafx.h"
#include <gl\GL.h>
#include "gl/glext.h"
#include "View.h"
#include <cstring>
#include <Windows.h>
#include "GlBase.h"

#pragma comment(lib, "freetype.lib")

GLuint prog;

unsigned char* readBMP(char* filename, int &width, int &height)
{
	int i;
	FILE* f = fopen(filename, "rb");
	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

	// extract image height and width from header
	width = *(int*)& info[18];
	height = *(int*)& info[22];

	int size = 3 * width * height;
	unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
	fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
	fclose(f);

	for (i = 0; i < size; i += 3)
	{
		unsigned char tmp = data[i];
		data[i] = data[i + 2];
		data[i + 2] = tmp;
	}

	return data;
}

LRESULT CView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
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

	if (FT_New_Face(_ftLib, "segoepr.ttf", 0, &_ftFace))
	{
		return false;
	}
	

	FT_Set_Pixel_Sizes(_ftFace, 0, 24);

	if (FT_Load_Char(_ftFace, '0', FT_LOAD_RENDER) || FT_Render_Glyph(_ftFace->glyph, FT_RENDER_MODE_NORMAL))
	{
		return false;
	}
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &_tex);
	glBindTexture(GL_TEXTURE_2D, _tex);
	glActiveTexture(GL_TEXTURE0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width = 0;
	int height = 0;
	unsigned char * data = readBMP("D:/img/70908446147.bmp", width, height);

	//const FT_Bitmap &bmp = _ftFace->glyph->bitmap;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	delete[] data;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GlBase base;
	prog = base.CreateShaderProg("vert.txt", "frag.txt");
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

	glClearColor(0, 0.8, 0, 1.0);
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

	glColor3f(1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, _tex);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0, 1.0); glVertex3f(-1.0, 1.0, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, -1.0, 0.0);
	glTexCoord2f(0.0, 0.0); glVertex3f(1.0, -1.0, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(1.0, 1.0, 0.0);
	glEnd();


	glUseProgram(prog);

	glUseProgram(0);

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