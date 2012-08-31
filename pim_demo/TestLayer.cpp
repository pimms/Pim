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

	animTimer = 0.f;
	animFrame = 0;

	color = Pim::Color(0.2f, 0.5f, 1.f, 1.f);

	// Initiate the sprite!					
	player = new Pim::Sprite("player.png");			// We are loading this file.		
	player->anchor = Pim::Vec2(0.5f, 0.f);		
	player->position = Pim::Vec2(25.f, 67.f);
	player->rect = Pim::Rect(0,0,50,80);			
	addChild(player);								// The sprite is not in Pim's control, and will be deleted automatically.
													// Unles of course, you do it yoruself.
													// Like so: removeChild(sprite1, true);

	gun = new Pim::Sprite("gun.png");
	gun->anchor = Pim::Vec2(0.2f, 0.5f);
	gun->position = Pim::Vec2(0.f, 30.f);
	player->addChild(gun);

	grass = new Pim::Sprite("grass.png");
	grass->anchor = Pim::Vec2(0.f, 0.f);
	addChild(grass);


	Pim::Input::getSingleton()->bindKey("right", Pim::Key::K_D);	// Binding a key will allow you to query for the name
	Pim::Input::getSingleton()->bindKey("left", Pim::Key::K_A);		// you assigned the key, instead of the key itself.
	Pim::Input::getSingleton()->bindKey("jump", Pim::Key::K_SPACE);	// Hopefully, this will simplify key binding.
}


void TestLayer::keyEvent(Pim::KeyEvent &evt)
{
	// Move the player around!
	if (evt.isKeyDown("right"))
	{
		playerVelocity.x += 1.f;
		player->scale.x = 1.f;
		gun->scale.x = 1.f;
	}
	else if (evt.isKeyDown("left"))
	{
		playerVelocity.x -= 1.f;
		player->scale.x = -1.f;
		gun->scale.x = -1.f;
	}
	if (evt.isKeyFresh("jump"))
		playerVelocity.y = 1.f;

	// Note that you can call evt.isKeyFresh(Pim::Key::K_xxx) to see if this is 
	// the first dispatched event where this button was pressed.
}
void TestLayer::mouseEvent(Pim::MouseEvent &evt)
{
	mousePos = evt.getPosition();
}
void TestLayer::update(float dt)
{
	// Handle game logic here //

	// Rotate dat gun
	gun->rotation = (mousePos-gun->getWorldPosition()).angleBetween(Pim::Vec2(1,0));
	if (gun->scale.x < 0)
		gun->rotation += 180.f;


	// Handle the animation
	if (playerVelocity.x != 0.f)
	{
		animTimer += dt;
		if (animTimer >= 0.1f)
		{
			animFrame++;
			animTimer -= 0.1f;
			if (animFrame == 3)
				animFrame = 0;
		}
	}
	else
	{
		animFrame = 0;
	}

	// Update the player frame
	player->rect = Pim::Rect(50*animFrame,0,50,80);

	// Move the player left and right
	player->position.x += playerVelocity.x * dt * 100.f;
	playerVelocity.x = 0.f;
	
	// Move the player up and down
	player->position.y += playerVelocity.y * dt * 100.f;

	// Simulate gravity
	playerVelocity.y -= dt * 5.f;
	
	// Is player moving below the ground?
	if (player->position.y < 67.f)
		player->position.y = 67.f;
}