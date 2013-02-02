#ifdef WIN32
#pragma once

#include "PimRenderWindow.h"

namespace Pim {
	class RenderWindowWIN : public RenderWindowBase {
	public:
		HWND						GetHwnd() const;

	protected:
		friend class GameControl;
		friend class LightingSystem;

		HINSTANCE					hInstance;
		HWND						hWnd;

									RenderWindowWIN(WinStyle::CreationData &data);
									~RenderWindowWIN();
		bool						SetupWindow(WinStyle::CreationData &data);
		void						KillWindow();
		void						SetWindowStyle(WinStyle::WinStyle style);
	};
}
#endif /* WIN32 */