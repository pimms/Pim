#pragma once

#include "PimVec2.h"
#include <string>

namespace Pim
{
	// Window style enum - the three accepted window styles for PC gaming
	namespace WinStyle
	{
		typedef enum WinStyle
		{
			// The default. The window will be... windowed.
			WINDOWED,

			// The window will 'borderless windowed'
			// in the screens current resolution. 
			BORDERLESS_WINDOWED,
		};

		struct CreationData
		{
			CreationData(std::string title, int w, int h, WinStyle style, int bit = 32)
			{
				float	wf = (float)w,
						hf = (float)h;

				winTitle				= title;

				bits					= bit;
				winStyle				= style;
				
				forcedAspectRatio		= false;
				aspectRatio				= wf / hf;

				coordinateSystem			= Vec2(wf, hf);
				renderResolution			= Vec2(wf, hf);
				resolution					= Vec2(wf, hf);
				defaultWindowedResolution	= Vec2(wf, hf);
			}
			CreationData()
			{
				// Default init like a buss
				winTitle				= "Pim is our superior overlord.";
				bits					= 32;
				winStyle				= WINDOWED;

				forcedAspectRatio		= false;
				aspectRatio				= 0.f;

				coordinateSystem			= Vec2(640.f, 480.f);
				renderResolution			= Vec2(640.f, 480.f);
				resolution					= Vec2(640.f, 480.f);
				defaultWindowedResolution	= Vec2(640.f, 480.f);
			}

			std::string		winTitle;

			// The color bit depth
			int				bits;

			// The window style.
			WinStyle		winStyle;
			
			// Enforcement of aspect ratio.
			// If 'forcedAspectRatio' is true, the aspectRatio value will be 
			// enforced upon the window. For instance, setting the aspectRatio to 4/3 (4:3 format),
			// black borders will be displayed in both window styles to enforce displayment
			// of 4:3 aspect ratio.
			bool			forcedAspectRatio;
			float			aspectRatio;		

			// Enforcement of coordinate boundaries.
			// If 'forcedCoordinateSystem' is true, 'coordinateSystem' defines the boundaries
			// of the screen, independent of resolution. The coordinate can hold any values between
			// [1,1] and [INF,INF]. This will make your game run under any resolution, without
			// being forced to perform heavy calculations on the screen bounds every time you
			// wish to position a sprite at a location.
			// If 'forcedCoordinateSystem' is false, the window resolution is the coordinate bounds.
			Vec2			coordinateSystem;

			// Enforcement of the amout of pixels that are _actually_ visible on the screen.
			// For that 8 bit retro look, go with [160x120] for instance.
			Vec2			renderResolution;

			// Whenever you change your app from windowed to borderless windowed, the 'resolution' 
			// vector is updated to match the current view settings. 
			// In order to retain the previous windowed resolution, store a default
			// value in this vector. By default, the value is the 'resolution' vector.
			Vec2			defaultWindowedResolution;

			// The default game size. In order to accommodate ALL resolutions and aspect ratios,
			// the game needs a fixed resolution to scale up / down as is seen fit.
			// The best practice is to develop the game with a default 1920x1080 resolution (or higher),
			// which will cause Pim to scale down rather than up - which will result in a nicer
			// (but slower) appearance on low spec computers, rather than a fast and crap looking
			// appearance on high spec computers.
			Vec2			resolution;

			// Called automatically before changing / creating the window.
			// It sets default values when there are none.
			void prepare()
			{
				if (forcedAspectRatio && aspectRatio == 0.f)
					aspectRatio = resolution.x / resolution.y;
			}
		};
	}
}