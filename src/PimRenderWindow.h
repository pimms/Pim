#pragma once

#include "PimWinStyle.h"

#include <Windows.h>
#include <gl/GL.h>

#include <string>

#include <functional>

namespace Pim
{
	
	class GameControl;
	class LightingSystem;

	class RenderWindow
	{
	public:
		RenderWindow(WinStyle::CreationData &data);
		~RenderWindow();

		// Print OpenGL errors. Useful if you're drawing stuff manually.
		// Pass by a an identifier, for instance "PRE-RENDER".
		// The output will be: "OpenGL error (PRE-RENDER): <gl_error>" if an error
		// has occured.
		void printOpenGLErrors(std::string identifier);

		HWND getHwnd();

		Vec2 getOrtho();		// Get the render dimensions in pixels
		Vec2 getOrthoOffset();	// Get the offset of the ortho projection (black borders)

	private:
		friend class GameControl;
		friend class LightingSystem;

		bool createWindow(WinStyle::CreationData &data);
		void resizeWindow(int wnew, int hnew);
		bool initOpenGL();
		void killWindow();

		void setCreationData(WinStyle::CreationData &data);
		void setWindowStyle(WinStyle::WinStyle style);

		// Call render on the top level layer
		void renderFrame();

	
		HDC			hDC;		// Private device context
		HGLRC		hRC;		// Rendering context
		HWND		hWnd;		// Window handle
		HINSTANCE	hInstance;	// Application instance

		WinStyle::CreationData	winData;
		
		enum BORDERPOS
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

		// This vector contains the border dimension data. 
		// The total screen size is (ortho + orthoOff*2)
		Vec2		orthoOff;

		// The border placement (NONE, HOR, VER)
		BORDERPOS	bpos;	

		// The border dimensions (height or width, depending on bpos)
		int			bdim;	
	};

}