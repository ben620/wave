#include "stdafx.h"
#include "View.h"
#include <gl\GL.h>

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

	return true;
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

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(0, 1, 0);
	glBegin(GL_TRIANGLES);
	glVertex3f(-1, 0, 0);
	glVertex3f(0, 0.5, 0);
	glVertex3f(0, 0.8, 0);
	glVertex3f(-1, 0.8, 0);
	glEnd();

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