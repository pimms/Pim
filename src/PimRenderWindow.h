#pragma once

#include "PimWinStyle.h"

#include <Windows.h>
#include <gl/GL.h>
#include <string>
#include <functional>

namespace Pim {
	class GameControl;
	class LightingSystem;

	class RenderWindow {
	private:
		friend class GameControl;
		friend class LightingSystem;

	public:
									RenderWindow(WinStyle::CreationData &data);
									~RenderWindow();
		Vec2						GetOrtho() const;
		Vec2						GetOrthoOffset() const;
		HWND						GetHwnd() const;
		void						PrintOpenGLErrors(string identifier);

	private:
		enum BORDERPOS {
			NONE,
			VER,
			HOR,
		};

		HDC							hDC;		// Private device context
		HGLRC						hRC;		// Rendering context
		HWND						hWnd;		// Window handle
		HINSTANCE					hInstance;	// Application instance
		WinStyle::CreationData		winData;
		Vec2						scale;
		Vec2						ortho;		// Screen pixel dimension
		Vec2						orthoOff;	// Border offset
		BORDERPOS					bpos;		// Border position
		int							bdim;		// Border dimensions

		bool						SetupWindow(WinStyle::CreationData &data);
		void						ResizeWindow(int wnew, int hnew);
		bool						InitOpenGL();
		void						KillWindow();
		void						SetCreationData(WinStyle::CreationData &data);
		void						SetWindowStyle(WinStyle::WinStyle style);
		void						RenderFrame(); 
	};
}