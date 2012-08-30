#pragma once
#include "Pim.h"
#include "TestNode.h"

class TestLayer : public Pim::Layer
{
public:
	// This is where we load our sprites, etc.
	void loadResources();

	// Overriding the virtual methods of GameNode
	void keyEvent(Pim::KeyEvent &evt);
	void update(float dt);

	// Let's add some sprites!
	Pim::Sprite *sprite1;
};

