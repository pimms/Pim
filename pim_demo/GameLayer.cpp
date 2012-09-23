#include "stdafx.h"
#include "GameLayer.h"
#include "Player.h"

void GameLayer::loadResources()
{
	world = new b2World(b2Vec2(0.f, -40.f));

	/*
		PLAYER
	*/
	player = new Player(world);
	player->setZOrder(1);
	addChild(player);

	/*
		NPC
		pos: 245.173782, 15.0149956
	*/
	npc = new Pim::Sprite("res/npc.png");
	npc->anchor = Pim::Vec2(0.5f, 0.f);
	npc->position = Pim::Vec2(245.173782f, 15.0149956f);
	addChild(npc);

	/*
		TERRAIN
	*/

	terrain = new Pim::Sprite("res/ter.png");
	terrain->setZOrder(2);
	terrain->anchor = Pim::Vec2(0.f, 0.f);
	addChild(terrain);

	// Create the terrain shape
	b2Vec2 vert[] = {
		b2Vec2(400.f, 0.f),
		b2Vec2(400.f, 15.f),
		b2Vec2(130.f, 15.f),
		b2Vec2(0.f, 3.f),
		b2Vec2(0.f, 0.f),
	};
	b2PolygonShape shape;
	shape.Set(vert, 5);

	b2FixtureDef fd;
	fd.shape = &shape;
	fd.density = 0.f;

	b2BodyDef bd;
	bd.type = b2_staticBody;

	terBody = world->CreateBody(&bd);
	terBody->CreateFixture(&fd);

	/*
		GUI
	*/
	uiLayer = new UILayer();
	uiLayer->loadResources();
	
	
	/*
		LIGHTING
	*/
	createLightingSystem(Pim::Vec2(192.f, 108.f));
	setLightingUnlitColor(Pim::Color(0.f, 0.f, 0.f, 0.8f));

	Pim::LightDef *lDef = new Pim::SmoothLightDef;
	lDef->radius = 60;
	lDef->innerColor = Pim::Color(0.f, 0.f, 0.f, 1.f);
	lDef->outerColor = Pim::Color(0.f, 0.f, 0.f, 0.0f);
	lDef->falloff = 1.f;
	player->lightPosition = Pim::Vec2(0.f, 7.f);
	addLight(player, lDef);

	lDef = new Pim::SmoothLightDef;
	lDef->radius = 60;
	lDef->innerColor = Pim::Color(1.f, 0.f, 0.f, 1.f);
	lDef->outerColor = Pim::Color(1.f, 1.f, 1.f, 0.f);
	lDef->falloff = 1.f;
	addLight(npc, lDef);

	/*
		OTHER
	*/

	color = Pim::Color(85.f/255.f, 127.f/255.f, 127.f/255.f, 1.f);
	listenFrame();
	listenMouse();
	listenKeys();
}

void GameLayer::update(float dt)
{
	player->update(dt);

	physicsUpdate(dt);

	Pim::Vec2 rel = player->position + position;

	if (rel.x < 75) 
	{
		position.x = -player->position.x + 75;
	}
	else if (rel.x > 117) 
	{
		position.x = -player->position.x + 117;
	}

	if (player->position.x < npc->position.x)
	{
		npc->scale = Pim::Vec2(-1.f, 1.f);
	}
	else
	{
		npc->scale = Pim::Vec2(1.f, 1.f);
	}

	uiLayer->interactPrompt->hidden = abs(player->position.x - npc->position.x) >= 20;
}
void GameLayer::postFrame()
{
	uiLayer->draw();

	return;
	debugDrawShape(player->body, tob2(position));
	debugDrawShape(terBody, tob2(position));
}

void GameLayer::physicsUpdate(float dt)
{
	world->Step(dt, 10, 10);

	player->position = toPim(player->body->GetPosition());
	player->rotation = player->body->GetAngle();
}
void GameLayer::mouseEvent(Pim::MouseEvent &evt)
{
	if (evt.isKeyFresh(Pim::Mouse::MouseButton::MBTN_RIGHT))
		player->rotation += 6.f;
}
void GameLayer::keyEvent(Pim::KeyEvent &evt)
{

}