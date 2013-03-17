#pragma once
#include "Pim.h"

class HUDLayer : public Pim::Layer, public Pim::ButtonCallback
{
public:
					HUDLayer();
	void			LoadResources();

	Pim::Button*	buttonPrev;
	Pim::Button*	buttonNext;
};
