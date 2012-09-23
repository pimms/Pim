#pragma once

#include "TestGame.h"

class Player : public Pim::Sprite
{
public:
	Player(b2World*);
	~Player();

	void update(float dt);

	void keyEvent(Pim::KeyEvent &evt);


	Pim::KeyEvent	*keyEvt;

	b2Body			*body;
};

