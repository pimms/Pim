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
		winData		= data;
	}
	RenderWindow::~RenderWindow()
	{
		killWindow();
	}

	Vec2 RenderWindow::getOrtho()
	{
		return ortho;
	}
	Vec2 RenderWindow::getOrthoOffset()
	{
		return orthoOff;
	}

	const HWND* RenderWindow::getHwnd()
	{
		return &hWnd;
	}

	bool RenderWindow::createWindow(WinStyle::CreationData &data)
	{
		winData = data;

		GLuint		pixelFormat;
		WNDCLASS	wc;
		DWORD		dwExStyle;
		DWORD		dwStyle;
		RECT		winRect;
		winRect.left   = 0;
		winRect.right  = (long)data.resolution.x;
		winRect.top	   = 0;
		winRect.bottom = (long)data.resolution.y;

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

		// INIT BY DEFAULT TO WINDOWED.
		// The window style is set to whatever you want once the window
		// is created.
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle	  = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

		// Gibberish values. 
		winRect.left   = 0;
		winRect.right  = 1;
		winRect.top	   = 0;
		winRect.bottom = 1;

		// Create the window
		if (!(hWnd = CreateWindowEx(dwExStyle,
									"pim",
									data.winTitle.c_str(),
									dwStyle,
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

		// Prepare the pixel format
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
			1,											// Stencil buffer is required
			0,											// No Auxiliary Buffer
			PFD_MAIN_PLANE,								// Main Drawing Layer
			0,											// Reserved
			0, 0, 0										// Layer Masks Ignored
		};

		// Get the device context
		if (!(hDC = GetDC(hWnd)))
		{
			killWindow();
			throw new Exception("Could not create a GL device context.");
			return false;
		}

		// Choose the pixel format
		if (!(pixelFormat = ChoosePixelFormat(hDC, &pfd)))
		{
			killWindow();
			throw new Exception("Can't find a suitable pixel format.");
			return false;
		}

		// Set the pixel format
		if (!SetPixelFormat(hDC,pixelFormat,&pfd))
		{
			killWindow();
			throw new Exception("Can't set the pixel format.");
			return false;
		}

		// Create the rendering context
		if (!(hRC = wglCreateContext(hDC)))
		{
			killWindow();
			throw new Exception("Can't create a rendering context.");
			return false;
		}

		// Make the rendering context active
		if (!wglMakeCurrent(hDC,hRC))
		{
			killWindow();
			throw new Exception("Can't activate the GL rendering context.");
			return false;
		}

		// Custom init of OpenGL
		if (!initOpenGL())
		{
			killWindow();
			throw new Exception("Initialization of OpenGL failed.");
			return false;
		}

		// Initate GLEW
		GLenum res = glewInit();
		if (res != GLEW_OK)
		{
			std::cout<<"ERROR INITATING GLEW:\n" <<glewGetErrorString(res) <<"\n";
			system("PAUSE");
			return false;
		}

		// Set the window style
		setWindowStyle(winData.winStyle);

		return true;
	}
	void RenderWindow::resizeWindow(int nw, int nh)
	{
		if (nh == 0)
			nh = 1;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glViewport(0,0,nw,nh);

		if (winData.forcedAspectRatio)
		{
			float rap = (float)nw/(float)nh;		// real aspect ratio

			float rw = (float)nw, rh = (float)nh;

			if (abs(rap - winData.aspectRatio) < 0.02f) // 0.02
			{
				bpos = NONE;
				bdim = 0;
			}
			else if (rap > winData.aspectRatio)			// Too wide
			{
				rw = nh * winData.aspectRatio;

				bpos = VER;
				bdim = (int)ceil((nw-rw)/2.f);
				orthoOff = Vec2((float)bdim, 0.f);
			}
			else if (rap < winData.aspectRatio)	// Too tall
			{
				rh = nw / winData.aspectRatio;

				bpos = HOR;
				bdim = (int)ceil((nh-rh)/2.f);
				orthoOff = Vec2(0.f, (float)bdim);
			}

			glOrtho((nw-rw)/-2.f, rw+(nw-rw)/2.f, (nh-rh)/-2.f, rh+(nh-rh)/2.f, 0, 1);
			ortho = Vec2(rw,rh);

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

		scale = ortho / winData.renderResolution;

	}
	bool RenderWindow::initOpenGL()
	{
		glEnable(GL_TEXTURE_2D);
		glShadeModel(GL_SMOOTH);
		glClearColor(0.f, 0.f, 0.f, 0.f);

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

	void RenderWindow::setCreationData(WinStyle::CreationData &data)
	{
		if (data.winStyle != winData.winStyle)
		{
			winData = data;
			setWindowStyle(data.winStyle);
		}

		if (data.forcedAspectRatio != winData.forcedAspectRatio ||
			data.aspectRatio != winData.aspectRatio ||
			data.renderResolution != winData.renderResolution)
		{
			resizeWindow((int)data.resolution.x, (int)data.resolution.y);
			winData = data;
		}

		//winData = data;
	}
	void RenderWindow::setWindowStyle(WinStyle::WinStyle style)
	{
		DWORD dwExStyle;
		DWORD dwStyle;
		RECT winRect;

		LONG scrnw = GetSystemMetrics(SM_CXSCREEN);
		LONG scrnh = GetSystemMetrics(SM_CYSCREEN);

		if (style == WinStyle::BORDERLESS_WINDOWED)
		{
			winRect.right	= scrnw;
			winRect.bottom	= scrnh;
			winRect.left	= 0;
			winRect.top		= 0;

			dwExStyle	= WS_EX_APPWINDOW;
			dwStyle		= WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

			// If the window is currently maximized, it needs to be unmaximized first.
			SendMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);

			// Move to zero in order for borderless windowed to cover the display
			MoveWindow(hWnd, 0, 0, winRect.right, winRect.bottom, FALSE);

			SetWindowLongPtr(hWnd, GWL_EXSTYLE,	dwExStyle);
			SetWindowLongPtr(hWnd, GWL_STYLE,	dwStyle);
			SetWindowPos(hWnd, HWND_TOP, winRect.left, winRect.top, 
										 winRect.right, winRect.bottom, 
								SWP_NOMOVE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
		}
		else if (style == WinStyle::WINDOWED)
		{
			dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			dwStyle	  = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

			winRect.right  = (long)winData.defaultWindowedResolution.x;
			winRect.bottom = (long)winData.defaultWindowedResolution.y;

			// Place the window somewhat in the center
			winRect.left   = (scrnw-winRect.right) / 2.f;
			winRect.top	   = (scrnh-winRect.bottom) / 3.f;

			SetWindowLongPtr(hWnd, GWL_EXSTYLE,	dwExStyle);
			SetWindowLongPtr(hWnd, GWL_STYLE,	dwStyle);
			SetWindowPos(hWnd, HWND_TOP, winRect.left, winRect.top, 
										 winRect.right, winRect.bottom, 
								SWP_NOMOVE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
			MoveWindow(hWnd, winRect.left, winRect.top, 0, 0, FALSE);

			// Adjust the size to the edges of the window
			RECT rcClient, rcWind;
			POINT ptDiff;
			GetClientRect(hWnd, &rcClient);
			GetWindowRect(hWnd, &rcWind);
			ptDiff.x = (rcWind.right - rcWind.left) - rcClient.right;
			ptDiff.y = (rcWind.bottom - rcWind.top) - rcClient.bottom;
			MoveWindow(hWnd,rcWind.left, rcWind.top, 
				winRect.right + ptDiff.x, winRect.bottom + ptDiff.y, TRUE);
		}

		// Just in case, set focus and enable the window
		SetFocus(hWnd);
		EnableWindow(hWnd, true);
	}

	void RenderWindow::renderFrame()
	{
#ifdef _DEBUG
		printOpenGLErrors("PRERENDER FRAME (something has gone quite wrong)");
#endif
		// Clear screen with the top layer's color
		Color c = Layer::getTopLayer()->getColor();
		glClearColor(c.r,c.g, c.b, c.a);
		glClear(GL_COLOR_BUFFER_BIT);		// Clear the screen
		glClearColor(0.f, 0.f, 0.f, 0.f);

		glColor3ub(255,255,255);			// Default overlay color (white)

		glLoadIdentity();					// Reset The Current view Matrix

		Layer::getTopLayer()->draw();		// Draw the top level layer first

		glLoadIdentity();					// Reload the identity matrix

		if (bpos == VER)
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
		}
		else if (bpos == HOR)
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