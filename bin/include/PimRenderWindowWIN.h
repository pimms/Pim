
#pragma once

#include "PimRenderWindow.h"

namespace Pim {
	class RenderWindowWIN : public RenderWindowBase {
	public:
		HWND						GetHwnd() const;

	protected:
		friend class GameControl;
		friend class LightingSystem;

		

									RenderWindowWIN(WinStyle::CreationData &data);
									~RenderWindowWIN();
		bool						SetupWindow(WinStyle::CreationData &data);
		void						KillWindow();
		void						SetWindowStyle(WinStyle::WinStyle style);
	};
}