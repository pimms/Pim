#pragma once

#ifdef WIN32
#include "Pim.h"
#elif defined(__APPLE__)
#include <Pim/Pim.h>
#endif

class HUDLayer : public Pim::Layer, public Pim::ButtonCallback
{
public:
					HUDLayer();
	void			LoadResources();

	Pim::Button*	buttonPrev;
	Pim::Button*	buttonNext;
};
