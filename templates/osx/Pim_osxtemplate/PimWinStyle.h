#pragma once

#include "PimVec2.h"
#include <string>

namespace Pim {
	namespace WinStyle {
		enum WinStyle {
			WINDOWED,
			BORDERLESS_WINDOWED,
		};

		struct CreationData {
			string			winTitle;
			int				bits;
			WinStyle		winStyle;

			bool			forcedAspectRatio;
			float			aspectRatio;

			// Enforcement of coordinate boundaries.
			// If 'forcedCoordinateSystem' is true, 'coordinateSystem' defines the boundaries
			// of the screen, independent of resolution. The coordinate can hold any values between
			// [1,1] and [INF,INF]. This will make your game run under any resolution, without
			// being forced to perform heavy calculations on the screen bounds every time you
			// wish to position a sprite at a location.
			Vec2			coordinateSystem;

			// Scales sprites to match the set render resolution rather than setting the ortho
			// to the render-res. High quality shadows and shaders may still be used :-)
			Vec2			renderResolution;

			// Whenever you change your app from windowed to borderless windowed, the 'resolution'
			// vector is updated to match the current view settings.
			// In order to retain the previous windowed resolution, store a default
			// value in this vector. By default, the value is the 'resolution' vector.
			Vec2			defaultWindowedResolution;

			// The default game size. In order to accommodate ALL resolutions and aspect ratios,
			// the game needs a fixed resolution to scale up / down as is seen fit.
			Vec2			resolution;
			
							CreationData(const string title, int w, int h, 
										 WinStyle style, int bit = 32);
							CreationData();
			void			Prepare();
		};
	}
}