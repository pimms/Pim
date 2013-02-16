#include "PimInternal.h"
#include "PimRenderWindow.h"

#include "PimGameControl.h"
#include "PimAssert.h"
#include "PimLayer.h"
#include "PimScene.h"

#include <stdlib.h>

namespace Pim {

	/*
	=====================
	RenderWindow::RenderWindow
	=====================
	*/
	RenderWindow::RenderWindow(WinStyle::CreationData &data) {
		surface		= NULL;
	}

	/*
	=====================
	RenderWindow::~RenderWindow
	=====================
	*/
	RenderWindow::~RenderWindow() {
		
	}

	/*
	=====================
	RenderWindow::GetOrtho
	=====================
	*/
	Vec2 RenderWindow::GetOrtho() const {
		return ortho;
	}

	/*
	=====================
	RenderWindow::GetOrthoOffset
	=====================
	*/
	Vec2 RenderWindow::GetOrthoOffset() const {
		return orthoOff;
	}

	/*
	=====================
	RenderWindow::PrintOpenGLErrors
	=====================
	*/
	void RenderWindow::PrintOpenGLErrors(string identifier) const {
		GLenum en = glGetError();
		if (en != GL_NO_ERROR) {
			cout<<"OpenGL error (" <<identifier <<"): ";

			switch (en) {
			case GL_INVALID_ENUM:
				cout<<"GL_INVALID_ENUM";
				break;
			case GL_INVALID_VALUE:
				cout<<"GL_INVALID_VALUE";
				break;
			case GL_INVALID_OPERATION:
				cout<<"GL_INVALID_OPERATION";
				break;
			case GL_OUT_OF_MEMORY:
				cout<<"GL_OUT_OF_MEMORY";
				break;
			case GL_STACK_UNDERFLOW:
				cout<<"GL_STACK_UNDERFLOW";
				break;
			case GL_STACK_OVERFLOW:
				cout<<"GL_STACK_OVERFLOW";
				break;
			default:
				cout<<"UNDEFINED GL ERROR";
			}

			cout<<"\n";
		}
	}

	/*
	=====================
	RenderWindow::SetupWindow
	=====================
	*/
	bool RenderWindow::SetupWindow(WinStyle::CreationData &data) {
		const SDL_VideoInfo *video;
		
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			printf("Failed to initialize video\n");
			return false;
		}

		video = SDL_GetVideoInfo();
		if (!video) {
			printf("Could not retreive video info\n");
			return false;
		}

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE,            8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,          8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,           8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,          8);
 
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,          16);
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,         32);
 
		SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE,      8);
		SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE,    8);
		SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE,     8);
		SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE,    8);
 
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,  1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,  2);

	    if (SDL_SetVideoMode(
			data.resolution.x, 
			data.resolution.y, data.bits, 
			SDL_OPENGL | SDL_HWSURFACE | SDL_RESIZABLE) == 0) {
		   printf("Failed to setup video mode\n");
		   return false;
		}

	    // Initate GLEW
		GLenum res = glewInit();
		if (res != GLEW_OK) {
			cout<<"ERROR INITATING GLEW:\n" <<glewGetErrorString(res) <<"\n";
			system("PAUSE");
			return false;
		}

		SDL_WM_SetCaption(data.winTitle.c_str(), NULL);
		winData = data;

		ResizeWindow((int)winData.resolution.x, (int)winData.resolution.y);

	    return InitOpenGL();

		/*
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

		instanceHandle		= GetModuleHandle(NULL);				// Window instance
		wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw on resize
		wc.lpfnWndProc		= (WNDPROC)WndProc;						// msg callback
		wc.cbClsExtra		= 200;									// No extra window data
		wc.cbWndExtra		= 200;									// No extra window data
		wc.hInstance		= instanceHandle;						// Pass the instance
		wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Default icon
		wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Default cursor
		wc.hbrBackground	= NULL;									// No GL background
		wc.lpszMenuName		= NULL;									// No menu please
		wc.lpszClassName	= "pim";								// Set the class name

		if (!RegisterClass(&wc)) {
			throw new exception("Failed to register the window class.");
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
		if (!(windowHandle = CreateWindowEx(dwExStyle,
									"pim",
									data.winTitle.c_str(),
									dwStyle,
									0, 0,
									winRect.right,
									winRect.bottom,
									NULL,
									NULL,
									instanceHandle,
									NULL))) {
			KillWindow();
			throw new exception("Window creation error.");
			return false;
		}

		// Prepare the pixel format
		static	PIXELFORMATDESCRIPTOR pfd = {			// pfd Tells Windows How We Want Things To Be
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
		if (!(devCtx = GetDC(windowHandle))) {
			KillWindow();
			throw new exception("Could not create a GL device context.");
			return false;
		}

		// Choose the pixel format
		if (!(pixelFormat = ChoosePixelFormat(devCtx, &pfd))) {
			KillWindow();
			throw new exception("Can't find a suitable pixel format.");
			return false;
		}

		// Set the pixel format
		if (!SetPixelFormat(devCtx,pixelFormat,&pfd)) {
			KillWindow();
			throw new exception("Can't set the pixel format.");
			return false;
		}

		// Create the rendering context
		if (!(renCtx = wglCreateContext(devCtx))) {
			KillWindow();
			throw new exception("Can't create a rendering context.");
			return false;
		}

		// Make the rendering context active
		if (!wglMakeCurrent(devCtx,renCtx)) {
			KillWindow();
			throw new exception("Can't activate the GL rendering context.");
			return false;
		}

		// Custom init of OpenGL
		if (!InitOpenGL()) {
			KillWindow();
			throw new exception("Initialization of OpenGL failed.");
			return false;
		}

		// Initate GLEW
		GLenum res = glewInit();
		if (res != GLEW_OK) {
			cout<<"ERROR INITATING GLEW:\n" <<glewGetErrorString(res) <<"\n";
			system("PAUSE");
			return false;
		}

		SetWindowStyle(data.winStyle);
		
		return true;
		*/
	}

	/*
	=====================
	RenderWindow::KillWindow
	=====================
	*/
	void RenderWindow::KillWindow() {
		surface = NULL;
		SDL_Quit();

		/*
		if (renCtx) {
			wglMakeCurrent(NULL,NULL);
			wglDeleteContext(renCtx);
			renCtx = NULL;
		}
		if (devCtx) {
			ReleaseDC(windowHandle,devCtx);
			devCtx = NULL;
		}
		if (windowHandle) {
			DestroyWindow(windowHandle);
			windowHandle = NULL;
		}

		UnregisterClass("pim", instanceHandle);
		instanceHandle = NULL;
		*/
	}

	/*
	=====================
	RenderWindow::SetWindowStyle
	=====================
	*/
	void RenderWindow::SetWindowStyle(WinStyle::WinStyle style) {


		/*
		DWORD dwExStyle;
		DWORD dwStyle;
		RECT winRect;

		LONG scrnw = GetSystemMetrics(SM_CXSCREEN);
		LONG scrnh = GetSystemMetrics(SM_CYSCREEN);

		if (style == WinStyle::BORDERLESS_WINDOWED) {
			winRect.right	= scrnw;
			winRect.bottom	= scrnh;
			winRect.left	= 0;
			winRect.top		= 0;

			dwExStyle	= WS_EX_APPWINDOW;
			dwStyle		= WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

			// If the window is currently maximized, it needs to be unmaximized first.
			SendMessage(windowHandle, WM_SYSCOMMAND, SC_RESTORE, 0);

			// Move to zero in order for borderless windowed to cover the display
			MoveWindow(windowHandle, 0, 0, winRect.right, winRect.bottom, FALSE);

			SetWindowLongPtr(windowHandle, GWL_EXSTYLE,	dwExStyle);
			SetWindowLongPtr(windowHandle, GWL_STYLE,	dwStyle);
			SetWindowPos(windowHandle, HWND_TOP, winRect.left, winRect.top,
						 winRect.right, winRect.bottom,
						 SWP_NOMOVE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
		} else if (style == WinStyle::WINDOWED) {
			dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			dwStyle	  = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

			winRect.right  = (long)winData.defaultWindowedResolution.x;
			winRect.bottom = (long)winData.defaultWindowedResolution.y;

			// Place the window somewhat in the center
			winRect.left   = (scrnw-winRect.right) / 2;
			winRect.top	   = (scrnh-winRect.bottom) / 3;

			SetWindowLongPtr(windowHandle, GWL_EXSTYLE,	dwExStyle);
			SetWindowLongPtr(windowHandle, GWL_STYLE,	dwStyle);
			SetWindowPos(windowHandle, HWND_TOP, winRect.left, winRect.top,
						 winRect.right, winRect.bottom,
						 SWP_NOMOVE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
			MoveWindow(windowHandle, winRect.left, winRect.top, 0, 0, FALSE);

			// Adjust the size to the edges of the window
			RECT rcClient, rcWind;
			POINT ptDiff;
			GetClientRect(windowHandle, &rcClient);
			GetWindowRect(windowHandle, &rcWind);
			ptDiff.x = (rcWind.right - rcWind.left) - rcClient.right;
			ptDiff.y = (rcWind.bottom - rcWind.top) - rcClient.bottom;
			MoveWindow(windowHandle,rcWind.left, rcWind.top,
					   winRect.right + ptDiff.x, winRect.bottom + ptDiff.y, TRUE);
		}

		// Just in case, set focus and enable the window
		SetFocus(windowHandle);
		EnableWindow(windowHandle, true);
		*/
	}
	
	/*
	=====================
	RenderWindow::ResizeWindow
	=====================
	*/
	void RenderWindow::ResizeWindow(int nw, int nh) {
		if (nh == 0) {
			nh = 1;
		}

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glViewport(0,0,nw,nh);

		if (winData.forcedAspectRatio) {
			float rap = (float)nw/(float)nh;		// real aspect ratio

			float rw = (float)nw, rh = (float)nh;

			if (abs(rap - winData.aspectRatio) < 0.02f) { // 0.02
				bpos = NONE;
				bdim = 0;
			} else if (rap > winData.aspectRatio) {		// Too wide
				rw = nh * winData.aspectRatio;

				bpos = VER;
				bdim = (int)ceil((nw-rw)/2.f);
				orthoOff = Vec2((float)bdim, 0.f);
			} else if (rap < winData.aspectRatio) {	// Too tall
				rh = nw / winData.aspectRatio;

				bpos = HOR;
				bdim = (int)ceil((nh-rh)/2.f);
				orthoOff = Vec2(0.f, (float)bdim);
			}

			// TODO: Include black borders in the orthographic projection
			glOrtho((nw-rw)/-2.f, rw+(nw-rw)/2.f, (nh-rh)/-2.f, rh+(nh-rh)/2.f, 0, 1);
			ortho = Vec2(rw,rh);

		} else {
			glOrtho(0, nw, 0, nh, 0, 1);
			ortho = Vec2((float)nw,(float)nh);
			orthoOff = Vec2(0.f, 0.f);
			bpos = NONE;
		}

		glMatrixMode(GL_MODELVIEW);

		GameControl::GetSingleton()->actualWinWidth  = nw;
		GameControl::GetSingleton()->actualWinHeight = nh;

		scale = ortho / winData.renderResolution;
	}

	/*
	=====================
	RenderWindow::InitOpenGL
	=====================
	*/
	bool RenderWindow::InitOpenGL() {
		glEnable(GL_TEXTURE_2D);
		glShadeModel(GL_SMOOTH);
		glClearColor(0.f, 0.f, 0.f, 0.f);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);

		glMatrixMode(GL_PROJECTION);
		glViewport(0, 0, winData.resolution.x, winData.resolution.y);
		glOrtho(0, winData.resolution.x, 0, winData.resolution.y, 1.f, -1.f);

		glMatrixMode(GL_MODELVIEW);

		return true;
	}

	/*
	=====================
	RenderWindow::SetCreationData
	=====================
	*/
	void RenderWindow::SetCreationData(WinStyle::CreationData &data) {
		if (data.winStyle != winData.winStyle) {
			winData = data;
			SetWindowStyle(data.winStyle);
		}

		if (data.forcedAspectRatio != winData.forcedAspectRatio
		||	data.aspectRatio != winData.aspectRatio 
		||	data.renderResolution != winData.renderResolution) {
			ResizeWindow((int)data.resolution.x, (int)data.resolution.y);
			winData = data;
		}

		//winData = data;
	}

	/*
	=====================
	RenderWindow::RenderFrame
	=====================
	*/
	void RenderWindow::RenderFrame() {
		#ifdef _DEBUG
		PrintOpenGLErrors("PRERENDER FRAME (Should never occur)");
		#endif /* _DEBUG */

		// Clear screen with the top layer's color
		Scene *s = GameControl::GetScene();

		glClearColor(s->color.r,s->color.g,s->color.b,s->color.a);
		glClear(GL_COLOR_BUFFER_BIT);		// Clear the screen
		glClearColor(0.f, 0.f, 0.f, 0.f);

		glColor3ub(255,255,255);			// Default overlay color (white)

		glLoadIdentity();					// Reset The Current view Matrix

		s->DrawScene();						// Render the scene

		if (bpos == VER) {
			glColor4ub(0,0,0,255);
			int ww = GameControl::GetSingleton()->actualWinWidth;
			int wh = GameControl::GetSingleton()->actualWinHeight;

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
		} else if (bpos == HOR) {
			glColor4ub(0,0,0,255);
			int ww = GameControl::GetSingleton()->actualWinWidth;
			int wh = GameControl::GetSingleton()->actualWinHeight;

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

		//SwapBuffers(devCtx);				// Swap the buffers to draw to screen
		SDL_GL_SwapBuffers();

		#ifdef _DEBUG
		PrintOpenGLErrors("POSTRENDER FRAME");
		#endif /* _DEBUG */
	}
}