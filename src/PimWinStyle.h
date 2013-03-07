#pragma once

#include "PimVec2.h"
#include <string>

namespace Pim {
	namespace WinStyle {
		enum WinStyle {
			WINDOWED,
			BORDERLESS_WINDOWED,
		};
		
		/**
		 @struct 		CreationData
		 @brief 		Defines the window properties required by Pim.
		 
		 @var 			CreationData::winTitle
		 				Set the title of your window
		 
		 @var 			CreationData::bits
		 @brief			DEPRECATED. 32 is used.
		 
		 @var 			CreationData::winStyle
		 @brief 		WINDOWED or BORDERLESS_WINDOWED (unimplemented, 0.9).
		 
		 @var 			CreationData::forcedAspectRatio
		 @brief 		If you wish to enforce the aspect ratio by adding black
		 				borders at the relevant side of the screen, you may do
		 				so by setting this variable to true. You must in that 
		 				case also give the @e aspectRatio variable a fitting value.
		 
		 @var 			CreationData::aspectRatio
		 @brief 		Given that @e forcedAspectRatio is true, the screen will
		 				be added black borders to ensure that the real aspect ratio
		 				of the displayed content is clamped to the desired ar.
		
		 @var 			CreationData::coordinateSystem
		 @brief 		This vector will describe the boundaries of the window in
		 				coordinates independent of the pixel resolution. [0,0] will
						always be in the bottom left, and the vector given by
		 				@e coordinateSystem is the top-right corner.
		 
		 @var 			CreationData::renderResolution
		 @brief 		When setting the renderResolution variable, the entire
						scene is scaled to match the desired resolution. If you
		 				wish to use low-resolution textures, this is the best 
		 				place to start.
		 
		 @var 			CreationData::defaultWindowedResolution
		 @brief 		WILL ARRIVE AT A LATER DATE. IGNORE FOR NOW.
		 
		 @var 			CreationData::resolution
		 @brief 		The initial resolution of the window, given that the 
		 				winStyle is WINDOWED.
		 
		 */
		struct CreationData {
			string			winTitle;
			int				bits;
			WinStyle		winStyle;

			bool			forcedAspectRatio;
			float			aspectRatio;

			Vec2			coordinateSystem;
			Vec2			renderResolution;
			
			Vec2			defaultWindowedResolution;
			Vec2			resolution;
			
							CreationData(const string title, int w, int h, 
										 WinStyle style, int bit = 32);
							CreationData();
			void			Prepare();
		};
	}
}