#include "StdAfx.h"
#include "PimRenderWindow.h"

#include "PimGameControl.h"
#include "PimException.h"
#include "PimLayer.h"

#include <stdlib.h>

namespace Pim
{

	// Function is defined in GameControl.cpp
	LRESULT	CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

	RenderWindow::RenderWindow(WinStyle::CreationData &data)
	{
		hDC			= NULL;
		hRC			= NULL;
		hWnd		= NULL;
		hInstance	= NULL;
		winData		= &data;
	}
	RenderWindow::~RenderWindow()
	{
		killWindow();
	}

	bool RenderWindow::createWindow(WinStyle::CreationData &data)
	{
		winData = &data;

		GLuint		pixelFormat;
		WNDCLASS	wc;
		DWORD		dwExStyle;
		DWORD		dwStyle;
		RECT		winRect;
		winRect.left   = (long)0;
		winRect.right  = (long)data.width;
		winRect.top	   = (long)0;
		winRect.bottom = (long)data.height;

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

		if (data.winStyle == WinStyle::FULLSCREEN)
		{
			// Get the screen's resolution.
			const HWND hDesktop = GetDesktopWindow();
			GetWindowRect(hDesktop, &winRect);

			DEVMODE dmScreenSettings;
			memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
			dmScreenSettings.dmSize			= sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth	= winRect.right;
			dmScreenSettings.dmPelsHeight	= winRect.bottom;
			dmScreenSettings.dmBitsPerPel	= data.bits;
			dmScreenSettings.dmFields		= DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

			if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL)
			{
				dwExStyle	= WS_EX_APPWINDOW;
				dwStyle		= WS_POPUP;	
			}
			else
			{
				MessageBox(NULL,"Fullscreen is not supported.\nChanging to windowed mode.","Fullscreen not supported",MB_OK | MB_ICONEXCLAMATION);
				data.winStyle = WinStyle::WINDOWED;
			}
		}
		else if (data.winStyle == WinStyle::BORDERLESS_FULLSCREEN)
		{
			dwExStyle = WS_EX_APPWINDOW;
			dwStyle = WS_POPUP;

			// Get the screen's resolution.
			const HWND hDesktop = GetDesktopWindow();
			GetWindowRect(hDesktop, &winRect);
		}

		// No else, any fault in previous settings will set winstyle to windowed.
		if (data.winStyle == WinStyle::WINDOWED)
		{
			dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			dwStyle	  = WS_OVERLAPPEDWINDOW;
		}

		if (!(hWnd = CreateWindowEx(dwExStyle,
									"pim",
									data.winTitle.c_str(),
									dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
									0, 0,
									winRect.right,
									winRect.bottom,
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
			data.bits,									// Select Our Color Depth
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

		if (!initOpenGL())
		{
			killWindow();
			throw new Exception("Initialization of OpenGL failed.");
			return false;
		}

		ShowWindow(hWnd,SW_SHOW);
		SetForegroundWindow(hWnd);
		SetFocus(GetDesktopWindow());
		SetFocus(hWnd);

		RECT rcClient, rcWind;
		POINT ptDiff;
		GetClientRect(hWnd, &rcClient);
		GetWindowRect(hWnd, &rcWind);
		ptDiff.x = (rcWind.right - rcWind.left) - rcClient.right;
		ptDiff.y = (rcWind.bottom - rcWind.top) - rcClient.bottom;
		MoveWindow(hWnd,rcWind.left, rcWind.top, winRect.right + ptDiff.x, winRect.bottom + ptDiff.y, TRUE);

		// FIX THIS SHIT. BOOKMARKED SO POST
		if (data.winStyle == WinStyle::WINDOWED)
			resizeWindow(winRect.right, winRect.bottom);
		else
			resizeWindow(winRect.right, winRect.bottom);

		// Initate GLEW
		GLenum res = glewInit();
		if (res != GLEW_OK)
		{
			std::cout<<"ERROR INITATING GLEW:\n" <<glewGetErrorString(res) <<"\n";
			system("PAUSE");
			return false;
		}

		return true;
	}
	void RenderWindow::resizeWindow(int nw, int nh)
	{
		if (nh == 0)
			nh = 1;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glViewport(0,0,nw,nh);

		if (winData->forcedAspectRatio)
		{
			float rap = (float)nw/(float)nh;		// real aspect ratio

			float rw = (float)nw, rh = (float)nh;

			if (abs(rap - winData->aspectRatio) < 0.02f) // 0.02
			{
				bpos = NONE;
				bdim = 0;
			}
			else if (rap > winData->aspectRatio)			// Too wide
			{
				rw = nh * winData->aspectRatio;

				bpos = VER;
				bdim = (int)ceil((nw-rw)/2.f);
				orthoOff = Vec2((float)bdim, 0.f);
			}
			else if (rap < winData->aspectRatio)	// Too tall
			{
				rh = nw / winData->aspectRatio;

				bpos = HOR;
				bdim = (int)ceil((nh-rh)/2.f);
				orthoOff = Vec2(0.f, (float)bdim);
			}

			glOrtho((nw-rw)/-2.f, rw+(nw-rw)/2.f, (nh-rh)/-2.f, rh+(nh-rh)/2.f, 0, 1);
			ortho = Vec2(rw,rh);

			std::cout<<"RENDER: " <<rw <<", " <<rh <<" - ";
			std::cout<<"WINDIM: " <<nw <<", " <<nh <<" - ";
			std::cout<<"BORDER: " <<bdim <<"\n";
		}
		else
		{
			glOrtho(0, nw, 0, nh, 0, 1);
			ortho = Vec2((float)nw,(float)nh);
			orthoOff = Vec2(0.f, 0.f);
			bpos = NONE;
		}
		
		glMatrixMode(GL_MODELVIEW);
		glDisable(GL_DEPTH_TEST);

		GameControl::getSingleton()->actualWinWidth  = nw;
		GameControl::getSingleton()->actualWinHeight = nh;

		if (winData->forcedRenderResolution)
		{
			scale = ortho / winData->renderResolution;
		}
		else
		{
			scale = ortho / winData->defaultResolution;
		}
	}
	bool RenderWindow::initOpenGL()
	{
		glEnable(GL_TEXTURE_2D);
		glShadeModel(GL_SMOOTH);
		glClearColor(0.f, 0.f, 0.f, 1.f);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);

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
#ifdef _DEBUG
		printOpenGLErrors("PRERENDER FRAME");
#endif
		// Clear screen with the top layer's color
		Color c = Layer::getTopLayer()->getColor();
		
		glClear(GL_COLOR_BUFFER_BIT);		// Clear the screen
		glClearColor(c.r, c.g, c.b, c.a);
		glColor3ub(255,255,255);			// Default overlay color (white)

		glLoadIdentity();					// Reset The Current view Matrix

		Layer::getTopLayer()->draw();		// Draw the top level layer first

		glLoadIdentity();					// Reload the identity matrix

		switch (bpos)
		{
			case VER:
			{
				glColor4ub(0,0,0,255);
				int ww = GameControl::getSingleton()->actualWinWidth;
				int wh = GameControl::getSingleton()->actualWinHeight;

				glDisable(GL_TEXTURE_2D);
				glBegin(GL_QUADS);
					// Left
					glVertex2i(-bdim-5, 0);
					glVertex2i(0, 0);
					glVertex2i(0, wh);
					glVertex2i(-bdim-5, wh);

					// Right
					glVertex2i(ww-bdim+5, 0);
					glVertex2i(ww-bdim-bdim, 0);
					glVertex2i(ww-bdim-bdim, wh);
					glVertex2i(ww-bdim+5, wh);
				glEnd();
				glEnable(GL_TEXTURE_2D);
				break;
			}

			case HOR:
			{
				glColor4ub(0,0,0,255);
				int ww = GameControl::getSingleton()->actualWinWidth;
				int wh = GameControl::getSingleton()->actualWinHeight;

				glDisable(GL_TEXTURE_2D);
				glBegin(GL_QUADS);
					// Top
					glVertex2i(0,wh-bdim+5);
					glVertex2i(ww,wh-bdim+5);
					glVertex2i(ww,wh-bdim-bdim);
					glVertex2i(0,wh-bdim-bdim);

					// Bottom
					glVertex2i(0,-bdim-5);
					glVertex2i(ww,-bdim-5);
					glVertex2i(ww,0);
					glVertex2i(0,0);
				glEnd();
				glEnable(GL_TEXTURE_2D);
				break;
			}

			case NONE: default:
				break;
		}

		// Dispatch post render messages
		GameControl::getSingleton()->dispatchPostrender();

		SwapBuffers(hDC);				// Swap the buffers to draw to screen

#ifdef _DEBUG
		printOpenGLErrors("POSTRENDER FRAME");
#endif
	}

	void RenderWindow::printOpenGLErrors(std::string identifier)
	{
		GLenum en = glGetError();
		if (en != GL_NO_ERROR)
		{
			std::cout<<"OpenGL error (" <<identifier <<"): ";

			switch (en)
			{
				case GL_INVALID_ENUM:
					std::cout<<"GL_INVALID_ENUM";
					break;
				case GL_INVALID_VALUE:
					std::cout<<"GL_INVALID_VALUE";
					break;
				case GL_INVALID_OPERATION:
					std::cout<<"GL_INVALID_OPERATION";
					break;
				case GL_OUT_OF_MEMORY:
					std::cout<<"GL_OUT_OF_MEMORY";
					break;
				case GL_STACK_UNDERFLOW:
					std::cout<<"GL_STACK_UNDERFLOW";
					break;
				case GL_STACK_OVERFLOW:
					std::cout<<"GL_STACK_OVERFLOW";
					break;
				default:
					std::cout<<"UNDEFINED GL ERROR";
			}

			std::cout<<"\n";
		}
	}

}