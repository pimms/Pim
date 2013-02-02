#ifdef WIN32

#include "PimRenderWindowWIN.h"
#include "PimGameControl.h"
#include "PimLightingSystem.h"

namespace Pim {

	/* Defined in GameControl.cpp */
	LRESULT	CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

	/*
	====================
	RenderWindowWIN::RenderWindowWIN
	====================
	*/
	RenderWindowWIN::RenderWindowWIN(WinStyle::CreationData &data)
	: RenderWindowBase(data) {
		hWnd		= NULL;
		hInstance	= NULL;
	}

	/*
	====================
	RenderWindowWIN::~RenderWindowWIN
	====================
	*/
	RenderWindowWIN::~RenderWindowWIN() {
		KillWindow();
	}

	/*
	=====================
	RenderWindowWIN::GetHwnd
	=====================
	*/
	HWND RenderWindowWIN::GetHwnd() const {
		return hWnd;
	}

	/*
	=====================
	RenderWindowWIN::SetupWindow
	=====================
	*/
	bool RenderWindowWIN::SetupWindow(WinStyle::CreationData &data) {
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
		if (!(devCtx = GetDC(hWnd))) {
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
	}

	/*
	=====================
	RenderWindowWIN::KillWindow
	=====================
	*/
	void RenderWindowWIN::KillWindow() {
		if (renCtx) {
			wglMakeCurrent(NULL,NULL);
			wglDeleteContext(renCtx);
			renCtx = NULL;
		}
		if (devCtx) {
			ReleaseDC(hWnd,devCtx);
			devCtx = NULL;
		}
		if (hWnd) {
			DestroyWindow(hWnd);
			hWnd = NULL;
		}

		UnregisterClass("pim", hInstance);
		hInstance = NULL;
	}

	/*
	=====================
	RenderWindowWIN::SetWindowStyle
	=====================
	*/
	void RenderWindowWIN::SetWindowStyle(WinStyle::WinStyle style) {
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
			SendMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);

			// Move to zero in order for borderless windowed to cover the display
			MoveWindow(hWnd, 0, 0, winRect.right, winRect.bottom, FALSE);

			SetWindowLongPtr(hWnd, GWL_EXSTYLE,	dwExStyle);
			SetWindowLongPtr(hWnd, GWL_STYLE,	dwStyle);
			SetWindowPos(hWnd, HWND_TOP, winRect.left, winRect.top,
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
}

#endif /* WIN32 */