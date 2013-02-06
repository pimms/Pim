#pragma once

#include "PimWinStyle.h"

#include <string>
#include <functional>

#include <Windows.h>
typedef HDC			DeviceContext;
typedef HGLRC		RenderingContext;

namespace Pim {
	class GameControl;
	class LightingSystem;

	class RenderWindow {
	protected:
		friend class GameControl;
		friend class LightingSystem;

	public:
									RenderWindow(WinStyle::CreationData &data);
									~RenderWindow();
		Vec2						GetOrtho() const;
		Vec2						GetOrthoOffset() const;
		void						PrintOpenGLErrors(string identifier) const;
		HWND						GetWindowHandle() const;

	protected:
		enum BORDERPOS {
			NONE,
			VER,
			HOR,
		};

		DeviceContext				devCtx;		// Handle for device context
		RenderingContext			renCtx;		// Rendering context
		WinStyle::CreationData		winData;
		Vec2						scale;
		Vec2						ortho;		// Screen pixel dimension
		Vec2						orthoOff;	// Border offset
		BORDERPOS					bpos;		// Border position
		int							bdim;		// Border dimensions
		HINSTANCE					instanceHandle;
		HWND						windowHandle;

		virtual bool				SetupWindow(WinStyle::CreationData &data);
		virtual void				KillWindow();
		virtual void				SetWindowStyle(WinStyle::WinStyle style);
		void						ResizeWindow(int wnew, int hnew);
		bool						InitOpenGL();
		void						SetCreationData(WinStyle::CreationData &data);
		void						RenderFrame(); 
	};
}