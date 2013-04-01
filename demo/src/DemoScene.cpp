#include "DemoScene.h"

#include "WelcomeLayer.h"
#include "LightLayer.h"
#include "SoundLayer.h"
#include "ActionLayer.h"
#include "ParticleLayer.h"

#define IDX_MAX 4
#define IDX_MIN 0



/*
==================
DemoScene::LoadResources
==================
*/
void DemoScene::LoadResources()
{
	layerIdx = 0;

	hud = new HUDLayer;
	AddLayer(hud);
	hud->SetZOrder(-2);

	color = Pim::Color(1.f, 1.f, 1.f, 1.f);

	layer = new WelcomeLayer;
	AddLayer(layer);

	ListenCommand("demoscene");
}

/*
==================
DemoScene::ButtonReleased
==================
*/
void DemoScene::ButtonReleased(Pim::Button *button)
{
	if (button == hud->buttonNext)
	{
		layerIdx++;
		if (layerIdx > IDX_MAX) {
			layerIdx = IDX_MIN;
		}
	}
	else if (button == hud->buttonPrev)
	{
		layerIdx--;
		if (layerIdx < IDX_MIN) {
			layerIdx = IDX_MAX;
		}
	}

	RemoveLayer(layer);

	switch (layerIdx)
	{
	case 0:
		layer = new WelcomeLayer;
		break;

	case 1:
		layer = new SoundLayer;
		break;

	case 2:
		layer = new LightLayer;
		break;

	case 3:
		layer = new ActionLayer;
		break;

	case 4:
		layer = new ParticleLayer;
		break;
	}

	AddLayer(layer);
}

/*
==================
DemoScene::HandleCommand
==================
*/
void DemoScene::HandleCommand(Pim::ConsoleCommand cmd)
{
	if (cmd.size() > 1)
	{
		if (cmd[1] == "next") {
			ButtonReleased(hud->buttonNext);
		} else if (cmd[1] == "prev") {
			ButtonReleased(hud->buttonPrev);
		}
	}
}