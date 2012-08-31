#include "StdAfx.h"
#include "PimRenderWindow.h"

#include "pim.h"
#include "PimGameControl.h"
#include "PimException.h"
#include "PimLayer.h"

#include <stdlib.h>

namespace Pim
{

	// Function is defined in GameControl.cpp
	LRESULT	CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

	// Singleton nullification
	RenderWindow* RenderWindow::singleton = NULL;


	RenderWindow::RenderWindow(std::string title, int w, int h)
	{
		if (singleton)
			throw new Exception("Unable to instantiate more than one RenderWindow at a time.");
		singleton = this;

		hDC			= NULL;
		hRC			= NULL;
		hWnd		= NULL;
		hInstance	= NULL;

		createWindow(title, w, h, 32);
	}
	RenderWindow::~RenderWindow()
	{
		singleton = NULL;
	}

	void RenderWindow::resizeWindow(int wnew, int hnew)
	{
		if (hnew == 0)
			hnew = 1;

		glViewport(0,0,wnew,hnew);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, wnew, 0, hnew, 0, 1);

		glMatrixMode(GL_MODELVIEW);
		glDisable(GL_DEPTH_TEST);
	}

	bool RenderWindow::createWindow(std::string &title, int w, int h, int bits)
	{
		GLuint		pixelFormat;
		WNDCLASS	wc;
		DWORD		dwExStyle;
		DWORD		dwStyle;
		RECT		winRect;
		winRect.left   = (long)0;
		winRect.right  = (long)w;
		winRect.top	   = (long)0;
		winRect.bottom = (long)h;

		hInstance			= GetModuleHandle(NULL);				// Window instance
		wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw on resize
		wc.lpfnWndProc		= (WNDPROC)WndProc;						// msg callback
		wc.cbClsExtra		= 200;									// No extra window data
		wc.cbWndExtra		= 200;									// No extra window data
		wc.hInstance		= hInstance;							// Pass the instance
		wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Default icon
		wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Default cursor
		wc.hbrBackground	= NULL;									// No GL background
		wc.lpszMenuName		= NULL;									// No menu please
		wc.lpszClassName	= "pim";								// Set the class name

		if (!RegisterClass(&wc))
		{
			throw new Exception("Failed to register the window class.");
			return false;
		}

		// if fullscreen... Nope!
		{
			dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			dwStyle	  = WS_OVERLAPPEDWINDOW;
		}

		AdjustWindowRectEx(&winRect, dwStyle, false, dwExStyle);

		if (!(hWnd = CreateWindowEx(dwExStyle,
									"pim",
									title.c_str(),
									dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
									0, 0,
									winRect.right-winRect.left,
									winRect.bottom-winRect.top,
									NULL, 
									NULL,
									hInstance,
									NULL)))
		{
			killWindow();
			throw new Exception("Window creation error.");
			return false;
		}

		static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
		{
			sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
			1,											// Version Number
			PFD_DRAW_TO_WINDOW |						// Format Must Support Window
			PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,							// Must Support Double Buffering
			PFD_TYPE_RGBA,								// Request An RGBA Format
			bits,										// Select Our Color Depth
			0, 0, 0, 0, 0, 0,							// Color Bits Ignored
			0,											// No Alpha Buffer
			0,											// Shift Bit Ignored
			0,											// No Accumulation Buffer
			0, 0, 0, 0,									// Accumulation Bits Ignored
			16,											// 16Bit Z-Buffer (Depth Buffer)  
			0,											// No Stencil Buffer
			0,											// No Auxiliary Buffer
			PFD_MAIN_PLANE,								// Main Drawing Layer
			0,											// Reserved
			0, 0, 0										// Layer Masks Ignored
		};

		if (!(hDC = GetDC(hWnd)))
		{
			killWindow();
			throw new Exception("Could not create a GL device context.");
			return false;
		}

		if (!(pixelFormat = ChoosePixelFormat(hDC, &pfd)))
		{
			killWindow();
			throw new Exception("Can't find a suitable pixel format.");
			return false;
		}

		if (!SetPixelFormat(hDC,pixelFormat,&pfd))
		{
			killWindow();
			throw new Exception("Can't set the pixel format.");
			return false;
		}

		if (!(hRC = wglCreateContext(hDC)))
		{
			killWindow();
			throw new Exception("Can't create a rendering context.");
			return false;
		}

		if (!wglMakeCurrent(hDC,hRC))
		{
			killWindow();
			throw new Exception("Can't activate the GL rendering context.");
			return false;
		}

		ShowWindow(hWnd,SW_SHOW);
		SetForegroundWindow(hWnd);
		SetFocus(hWnd);

		resizeWindow(w,h);

		if (!initOpenGL())
		{
			killWindow();
			throw new Exception("Initialization of OpenGL failed.");
			return false;
		}

		return true;
	}
	bool RenderWindow::initOpenGL()
	{
		glEnable(GL_TEXTURE_2D);
		glShadeModel(GL_FLAT);
		glClearColor(0.f, 0.f, 0.f, 1.f);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		return true;
	}

	void RenderWindow::killWindow()
	{
		if (hRC)
		{
			wglMakeCurrent(NULL,NULL);
			wglDeleteContext(hRC);
			hRC = NULL;
		}
		if (hDC)
		{
			ReleaseDC(hWnd,hDC);
			hDC = NULL;
		}
		if (hWnd)
		{
			DestroyWindow(hWnd);
			hWnd = NULL;
		}

		UnregisterClass("pim", hInstance);
		hInstance = NULL;
	}

	void RenderWindow::renderFrame()
	{
		// Clear screen with the top layer's color
		Color c = Layer::getTopLayer()->getColor();
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(c.r, c.g, c.b, c.a);

		glLoadIdentity();				// Reset The Current view Matrix

		Layer::getTopLayer()->draw();	// Draw the top level layer first

		SwapBuffers(hDC);				// Swap the buffers to draw to screen
	}

}