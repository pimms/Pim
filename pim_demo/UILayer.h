#pragma once

#include "Pim.h"

class UILayer : public Pim::Layer
{
public:
	void loadResources();

	Pim::Sprite		*interactPrompt;
};

