#pragma once
#include "Pim.h"
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
