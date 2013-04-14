#pragma once

#ifdef WIN32
#include "Pim.h"
#elif defined(__APPLE__)
#include <Pim/Pim.h>
#endif

#include "HUDLayer.h"

class DemoScene : public Pim::Scene, public Pim::ButtonCallback, public Pim::ConsoleListener
{
public:
	HUDLayer		*hud;
	Pim::Layer		*layer;
	int				layerIdx;

	void			LoadResources();
	void			ButtonReleased(Pim::Button *button);
	void			HandleCommand(Pim::ConsoleCommand cmd);
};
