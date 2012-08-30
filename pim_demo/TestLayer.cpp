#include "StdAfx.h"
#include "TestLayer.h"

#include <iostream>
#include <string>

void TestLayer::loadResources()
{
	// Schedule self for input lsitening and frame listening. 
	// TestLayer will be notified when a key or mouse event occurs,
	// and update() will be called every frame.
	listenInput();
	listenFrame();

	// Initiate the sprite!					
	sprite1 = new Pim::Sprite("pl.png");			// We are loading this file.		
	sprite1->position = Pim::Vec2(10.f, 50.f);		
	sprite1->scale = Pim::Vec2(10.f, 10.f);			
	sprite1->rect = Pim::Rect(0,0,24,12);			
	addChild(sprite1);								// The sprite is not in Pim's control, and will be deleted automatically.
													// Unles of course, you do it yoruself.
													// Like so: removeChild(sprite1, true);


	Pim::Input::getSingleton()->bindKey("right", Pim::Key::K_D);	// Binding a key will allow you to query for the name
	Pim::Input::getSingleton()->bindKey("left", Pim::Key::K_A);		// you assigned the key, instead of the key itself.
	Pim::Input::getSingleton()->bindKey("up", Pim::Key::K_W);		// Hopefully, this will simplify key binding.
	Pim::Input::getSingleton()->bindKey("down", Pim::Key::K_S);
}


void TestLayer::keyEvent(Pim::KeyEvent &evt)
{
	// Move the sprite around!
	if (evt.isKeyDown("right"))
		sprite1->position.x += 1.f;
	if (evt.isKeyDown("left"))
		sprite1->position.x -= 1.f;
	if (evt.isKeyDown("up"))
		sprite1->position.y += 1.f;
	if (evt.isKeyDown("down"))
		sprite1->position.y -= 1.f;

	// Note that you can call evt.isKeyFresh(Pim::Key::K_xxx) to see if this is 
	// the first dispatched event where this button was pressed.
}
void TestLayer::update(float dt)
{
	// Handle game logic here
}