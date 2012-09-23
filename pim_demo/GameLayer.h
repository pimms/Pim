#pragma once
#include "pim.h"
#include "Box2D/Box2D.h"
#include "UILayer.h"

class Player;

class GameLayer : public Pim::Layer
{
public:
	void loadResources();

	void update(float dt);
	void postFrame();

	void physicsUpdate(float dt);
	void mouseEvent(Pim::MouseEvent &evt);
	void keyEvent(Pim::KeyEvent &evt);

	Player			*player;
	Pim::Sprite		*terrain;
	Pim::Sprite		*npc;

	UILayer			*uiLayer;

	b2World			*world;
	b2Body			*terBody;
};

