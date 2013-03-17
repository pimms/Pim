#pragma once
#include "Pim.h"

class WelcomeLayer : public Pim::Layer
{
public:
	void		LoadResources();
	void		OnKeyEvent(Pim::KeyEvent &evt);
	void		OnControllerEvent(Pim::ControllerEvent &evt);
};

