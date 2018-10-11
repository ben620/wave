#include "stdafx.h"
#include "View.h"
#include <gl\GL.h>

LRESULT CView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	PIXELFORMATDESCRIPTOR pfd;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);//上述格式描述符的大小
	pfd.nVersion = 1;//版本号
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	//格式支持：窗口、OpenGL、双缓冲
	pfd.iPixelType = PFD_TYPE_RGBA;//申请RGBA格式
	pfd.cColorBits = 32;//选定色彩深度
	pfd.cRedBits = 0;//忽略色彩位
	pfd.cRedShift = 0;
	pfd.cGreenBits = 0;
	pfd.cGreenShift = 0;
	pfd.cBlueBits = 0;
	pfd.cBlueShift = 0;
	pfd.cAlphaBits = 0;//wuAlpha缓存
	pfd.cAlphaShift = 0;//忽略Shift Bit
	pfd.cAccumBits = 0;//无聚集缓存
	pfd.cAccumRedBits = 0;//忽略聚集位
	pfd.cAccumGreenBits = 0;
	pfd.cAccumBlueBits = 0;
	pfd.cAccumAlphaBits = 0;
	pfd.cDepthBits = 16;//16位 深度缓存（z-缓存）
	pfd.cStencilBits = 0;//无模板缓存
	pfd.cAuxBuffers = 0;//无辅助缓存
	pfd.iLayerType = PFD_MAIN_PLANE;//主绘图层
	pfd.bReserved = 0;//保留
	pfd.dwLayerMask = 0;//忽略遮罩层
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

	glShadeModel(GL_SMOOTH);                 // 启用阴影平滑  
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);    // 黑色背景  
	glClearDepth(1.0f);                      // 设置深度缓存  
	glEnable(GL_DEPTH_TEST);                 // 启用深度测试  
	glDepthFunc(GL_LEQUAL);                  // 所作深度测试的类型  
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);// 告诉系统对透视进行修正  

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