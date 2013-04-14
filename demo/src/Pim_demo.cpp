#ifdef WIN32
#include "Pim.h"
#elif defined(__APPLE__)
#include <Pim/Pim.h>
#endif

#include "DemoScene.h"
#include <iostream>

int main(int argc, char *args[]) {
	// Stack cheating - avoid reporting memory leaking from "cd".
	{
		Pim::GameControl gc;

		Pim::WinStyle::CreationData cd("PIM!", 800, 600, Pim::WinStyle::WINDOWED);
		cd.forcedAspectRatio	= true;
		cd.aspectRatio			= 4.f/3.f;
		cd.renderResolution		= Pim::Vec2(800, 600);
		cd.coordinateSystem		= Pim::Vec2(800, 600);

		gc.Go(new DemoScene, cd);
	}

#if defined(_DEBUG) && defined(WIN32)
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}

