#include "stdafx.h"
#include "UILayer.h"


void UILayer::loadResources()
{
	setImmovableLayer(true);

	interactPrompt = new Pim::Sprite("res/interact.png");
	interactPrompt->position = Pim::Vec2(96.f, 90.f);
	interactPrompt->hidden = true;
	addChild(interactPrompt);
}