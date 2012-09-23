#include "stdafx.h"
#include "Player.h"


Player::Player(b2World *world) : Pim::Sprite("res/player.png")
{
	listenKeys();

	keyEvt = NULL;

	anchor = Pim::Vec2(0.5f, 0.f);
	position = Pim::Vec2(26.4639111f, 5.45792150f);

	b2Vec2 vert[] = {
		b2Vec2(-4.f, 0.f),
		b2Vec2( 4.f, 0.f),
		b2Vec2( 4.f, 10.f),
		b2Vec2(-4.f, 10.f),
	};
	b2PolygonShape shape;
	shape.Set(vert, 4);

	b2FixtureDef fd;
	fd.shape = &shape;
	fd.density = 10.f;

	b2BodyDef bd;
	bd.type = b2_dynamicBody;

	body = world->CreateBody(&bd);
	body->CreateFixture(&fd);
	body->SetSleepingAllowed(false);
	body->SetFixedRotation(true);

	body->SetTransform(tob2(position), 0.f);
}


Player::~Player()
{

}

void Player::update(float dt)
{
	float y = body->GetLinearVelocity().y;
	body->SetLinearVelocity(b2Vec2(0.f, y));

	if (!keyEvt) return;

	if (keyEvt->isKeyDown(Pim::Key::K_D))
	{
		body->SetLinearVelocity(b2Vec2(30.f, y));
		scale = Pim::Vec2(1.f, 1.f);
	}
	else if (keyEvt->isKeyDown(Pim::Key::K_A))
	{
		body->SetLinearVelocity(b2Vec2(-30.f, y));
		scale = Pim::Vec2(-1.f, 1.f);
	}

	if (keyEvt->isKeyFresh(Pim::Key::K_SPACE))
		body->ApplyLinearImpulse(b2Vec2(0.f, 500.f), body->GetPosition());
}

void Player::keyEvent(Pim::KeyEvent &evt)
{
	keyEvt = &evt;
}