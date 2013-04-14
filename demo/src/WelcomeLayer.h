#pragma once

#ifdef WIN32
#include "Pim.h"
#elif defined(__APPLE__)
#include <Pim/Pim.h>
#endif

class WelcomeLayer : public Pim::Layer
{
public:
	void		LoadResources();
	void		OnKeyEvent(Pim::KeyEvent &evt);
	void		OnControllerEvent(Pim::ControllerEvent &evt);
};

