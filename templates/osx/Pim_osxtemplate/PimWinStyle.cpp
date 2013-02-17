#include "PimWinStyle.h"

namespace Pim {
	namespace WinStyle {
		/*
		=====================
		CreationData::CreationData
		=====================
		*/
		CreationData::CreationData(string title, int w, int h, WinStyle style, int bit) {
			float wf					= (float)w;
			float hf					= (float)h;
			winTitle					= title;
			bits						= bit;
			winStyle					= style;
			forcedAspectRatio			= false;
			aspectRatio					= wf / hf;
			coordinateSystem			= Vec2(wf, hf);
			renderResolution			= Vec2(wf, hf);
			resolution					= Vec2(wf, hf);
			defaultWindowedResolution	= Vec2(wf, hf);
		}

		/*
		=====================
		CreationData::CreationData
		=====================
		*/
		CreationData::CreationData()
		{
			// Default init like a buss
			winTitle					= "Pim is our superior overlord.";
			bits						= 32;
			winStyle					= WINDOWED;
			forcedAspectRatio			= false;
			aspectRatio					= 0.f;
			coordinateSystem			= Vec2(640.f, 480.f);
			renderResolution			= Vec2(640.f, 480.f);
			resolution					= Vec2(640.f, 480.f);
			defaultWindowedResolution	= Vec2(640.f, 480.f);
		}

		/*
		=====================
		CreationData::Prepare
		=====================
		*/
		void CreationData::Prepare() {
			if (forcedAspectRatio && aspectRatio == 0.f) {
					aspectRatio = resolution.x / resolution.y;
			}
		}
	}
}