#pragma once
#include "Pim.h"

class TestLayer : public Pim::Layer
{
public:
	// This is where we load our sprites, etc.
	void loadResources();

	// Overriding the virtual methods of GameNode
	void keyEvent(Pim::KeyEvent &evt);
	void mouseEvent(Pim::MouseEvent &evt);
	void update(float dt);

	// Let's add some sprites!
	Pim::Sprite *player;
	Pim::Sprite *gun;
	Pim::Sprite *grass;

	// Record movement in keyEvent, move player in update.
	Pim::Vec2 playerVelocity;

	// Save mouse position in mouseEvent, rotate gun in update
	Pim::Vec2 mousePos;

	float animTimer;
	int animFrame;
};

