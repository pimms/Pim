#pragma once

#include "PimWinStyle.h"

#include <string>
#include <functional>

namespace Pim {
	class GameControl;
	class LightingSystem;

	class RenderWindow {
	protected:
		friend class GameControl;
		friend class LightingSystem;

	public:
									RenderWindow();
									~RenderWindow();
		Vec2						GetOrtho() const;
		Vec2						GetOrthoOffset() const;
		void						PrintOpenGLErrors(string identifier) const;

	protected:
		enum BORDERPOS {
			NONE,
			VER,
			HOR,
		};

		SDL_Surface					*surface;	// The screen-surface
		WinStyle::CreationData		winData;
		Vec2						scale;
		Vec2						ortho;		// Screen pixel dimension
		Vec2						orthoOff;	// Border offset
		BORDERPOS					bpos;		// Border position
		int							bdim;		// Border dimensions

		virtual bool				SetupWindow(WinStyle::CreationData &data);
		virtual void				KillWindow();
		virtual void				SetWindowStyle(WinStyle::WinStyle style);
		void						ResizeWindow(int wnew, int hnew);
		bool						InitOpenGL();
		void						SetCreationData(WinStyle::CreationData &data);
		void						RenderFrame(); 
	};
}