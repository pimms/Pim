#pragma once

#include "PimWinStyle.h"

#include <string>
#include <functional>

namespace Pim {
	/**
	 @class 		RenderWindow
	 @brief 		The class managing the window. As of 0.9, RenderWindow is a
	 				wrapper around the SDL-window.
	 @details 		RenderWindow is a singleton object, and as all other Pim 
	 				singleton objects, it's managed by GameControl. 
	 
	 				This class is @e very internal, and is very rarely directly
	 				interacted with under normal conditions. 
	 
	 				Please do note that Pim is in no shape, way or form in a 
	 				normal condition, so interaction with RenderWindow may prove 
	 				inevitable.
	 
	 @todo 			Allow run-time modification of CreationData and WinStyle.
	 */
	
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

		SDL_Window					*window;  
		SDL_Renderer				*renderer;
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