// Pim_demo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Pim.h"
#include "TestLayer.h"

#include <iostream>

int _tmain(int argc, _TCHAR* argv[])
{
	// Game Control is the boss. It's a singleton object, and is in charge of your game.
	Pim::GameControl *gc = new Pim::GameControl("Pim demo",640,480);

	// Go initiates the main game loop, and instantiates objects before creating a window.
	// It must be passed a Pim::Layer.
	gc->go(new TestLayer);

#ifdef _DEBUG
		_CrtDumpMemoryLeaks();
#endif

	return 0;
}

