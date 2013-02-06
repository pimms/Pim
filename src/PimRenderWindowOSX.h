#ifdef __APPLE__

#pragma once

#include "PimRenderWindow.h"

namespace Pim {
	class RenderWindowOSX : public RenderWindow {
	public:

	protected:
		friend class GameControl;
		friend class LightingSystem;

									RenderWindowOSX(WinStyle::CreationData &data);
		bool						SetupWindow(WinStyle::CreationData &data);
		void						KillWindow();
		void						SetWindowStyle(WinStyle::WinStyle style);
	};
}

#endif /* __APPLE__ */