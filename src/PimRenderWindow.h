#pragma once

#include "PimWinStyle.h"

#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <GL/GLAux.h>

#include <string>

#include <functional>

namespace Pim
{
	
	class GameControl;

	class RenderWindow
	{
	public:
		RenderWindow(WinStyle::CreationData &data);
		~RenderWindow();

	private:
		friend class GameControl;

		bool createWindow(WinStyle::CreationData &data);
		void resizeWindow(int wnew, int hnew);
		bool initOpenGL();
		void killWindow();

		// Call render on the top level layer
		void renderFrame();

	
		HDC			hDC;		// Private device context
		HGLRC		hRC;		// Rendering context
		HWND		hWnd;		// Window handle
		HINSTANCE	hInstance;	// Application instance

		WinStyle::CreationData	*winData;
		
		typedef enum BORDERPOS
		{
			NONE,
			VER,
			HOR,
		};

		// The scale, post resize.
		Vec2		scale;	

		// The current window size (excluding black borders)
		// THIS VECTOR DISREGARDS THE FORCED COORDINATE SYSTEM
		// THIS VECTOR IS IN SCREEN PIXEL SIZE - NOT FORCED RENDER RESOLUTION
		Vec2		ortho;	

		// The border placement (NONE, HOR, VER)
		BORDERPOS	bpos;	

		// The border dimensions (height or width, depending on bpos)
		float		bdim;	
	};

}