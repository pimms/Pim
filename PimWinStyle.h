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

			// The window will be fullscreen, in the resolution you provide.
			FULLSCREEN,

			// The window will be 'windowed fullscreen', or 'borderless fullscreen',
			// in the screens current resolution. 
			BORDERLESS_FULLSCREEN,
		};

		struct CreationData
		{
			CreationData(std::string title, int w, int h, WinStyle style, int bit = 32)
			{
				winTitle	= title;

				// The window's dimensions. Only used in windowed mode.
				width		= w;
				height		= h;
				bits		= bit;
				winStyle	= style;
				
				forcedAspectRatio	= false;
				aspectRatio			= 0.f;

				forcedCoordinateSystem	= false;
				coordinateSystem		= Vec2(1.f,1.f);

				forcedRenderResolution	= false;
				renderResolution		= Vec2((float)w, (float)h);

				defaultResolution		= Vec2((float)w,(float)h);
			}
			CreationData()
			{
				// Default init like a buss
				winTitle	= "Pim is our superior overlord.";
				width		= 640;
				height		= 480;
				bits		= 32;
				winStyle	= WINDOWED;

				forcedAspectRatio	= false;
				aspectRatio			= 0.f;		// Invalid because fAR = false

				forcedCoordinateSystem	= false;
				coordinateSystem		= Vec2(1.f,1.f);

				forcedRenderResolution	= false;
				renderResolution		= Vec2((float)width, (float)height);

				defaultResolution		= Vec2(640.f,480.f);
			}

			std::string		winTitle;

			// The window dimensions. Only used for WINDOWED mode.
			int				width; 
			int				height;

			// The color bit depth - only used in FULLSCREEN mode.
			int				bits;

			// The window style.
			WinStyle		winStyle;
			
			// Enforcement of aspect ratio.
			// If 'forcedAspectRatio' is true, the aspectRatio value will be 
			// enforced upon the window. For instance, setting the aspectRatio to 4/3 (4:3 format),
			// will do the following in the different window styles:
			// WINDOWED: The window will be resized to match the aspect ratio. No black borders.
			// FULLSCREEN: Black borders will be displayed to match the current aspect ratio.
			// BORDERLESS_FULLSCREEN: Same as FULLSCREEN.
			bool			forcedAspectRatio;
			float			aspectRatio;		

			// Enforcement of coordinate boundaries.
			// If 'forcedCoordinateSystem' is true, 'coordinateSystem' defines the boundaries
			// of the screen, independent of resolution. The coordinate can hold any values between
			// [1,1] and [INF,INF]. This will make your game run under any resolution, without
			// being forced to perform heavy calculations on the screen bounds every time you
			// wish to position a sprite at a location.
			// If 'forcedCoordinateSystem' is false, the window resolution is the coordinate bounds.
			bool			forcedCoordinateSystem;
			Vec2			coordinateSystem;

			// Enforcement of the amout of pixels that are _actually_ visible on the screen.
			// For that 8 bit retro look, go with [160x120] for instance.
			bool			forcedRenderResolution;
			Vec2			renderResolution;

			// The default game size. In order to accommodate ALL resolutions and aspect ratios,
			// the game needs a fixed resolution to scale up / down as is seen fit.
			// The best practice is to develop the game with a default 1920x1080 resolution (or higher),
			// which will cause Pim to scale down rather than up - which will result in a nicer
			// (but slower) appearance on low spec computers, rather than a fast and crap looking
			// appearance on high spec computers.
			Vec2			defaultResolution;

			// Called automatically before changing / creating the window.
			// It sets default values when there are none.
			void prepare()
			{
				if (width != 640 && height != 480 && defaultResolution == Vec2(640.f, 480.f))
					defaultResolution = Vec2((float)width, (float)height);

				if (forcedCoordinateSystem && coordinateSystem == Vec2(0.f, 0.f))
					coordinateSystem = defaultResolution;

				if (forcedAspectRatio && aspectRatio == 0.f)
					aspectRatio = (float)width / (float)height;
			}
		};
	}
}