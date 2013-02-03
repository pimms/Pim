#pragma once

#include "PimWinStyle.h"

#include <gl/GL.h>
#include <string>
#include <functional>

#ifdef WIN32
	#include <Windows.h>
	typedef HDC			DeviceContext;
	typedef HGLRC		RenderingContext;
#endif /* WIN32 */

namespace Pim {
	class GameControl;
	class LightingSystem;

	class RenderWindowBase {
	protected:
		friend class GameControl;
		friend class LightingSystem;

	public:
									RenderWindowBase(WinStyle::CreationData &data);
									~RenderWindowBase();
		Vec2						GetOrtho() const;
		Vec2						GetOrthoOffset() const;
		void						PrintOpenGLErrors(string identifier);

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

		virtual bool				SetupWindow(WinStyle::CreationData &data)	= 0;
		virtual void				KillWindow()								= 0;
		virtual void				SetWindowStyle(WinStyle::WinStyle style)	= 0;
		void						ResizeWindow(int wnew, int hnew);
		bool						InitOpenGL();
		void						SetCreationData(WinStyle::CreationData &data);
		void						RenderFrame(); 
	};
}