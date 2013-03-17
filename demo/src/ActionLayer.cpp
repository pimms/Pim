#include "ActionLayer.h"

/*
=====================
ActionLayer::ActionLayer
=====================
*/
ActionLayer::ActionLayer(void)
{
	time = 0.f;
}

/*
=====================
ActionLayer::~ActionLayer
=====================
*/
ActionLayer::~ActionLayer(void)
{
	Pim::GameControl::GetSingleton()->LimitFrame(0);
}

/*
=====================
ActionLayer::LoadResources
=====================
*/
void ActionLayer::LoadResources()
{
	ListenFrame();
	//Pim::GameControl::getSingleton()->limitFrame(5);

	// Move To
	for (int x = -1; x <= 1; x += 2)
	{
		for (int y = -1; y <= 1; y += 2)
		{
			Pim::Sprite *sprite = new Pim::Sprite("lighttiles.png");
			sprite->position = Pim::Vec2(400.f + 100.f*x, 300.f + 100.f*y);
			AddChild(sprite);

			Pim::MoveToAction *action = new Pim::MoveToAction(Pim::Vec2(400.f, 300.f), 1.f);
			sprite->RunAction(action);
		}
	}

	// Move By
	for (int i = 0; i < 2; i++)
	{
		Pim::Sprite *sprite = new Pim::Sprite("lighttiles.png");
		sprite->position = Pim::Vec2( 50.f, 50.f + 500.f * i);
		AddChild(sprite);

		int fac = (!i) ? (1) : (-1);
		Pim::MoveByAction *action = new Pim::MoveByAction(Pim::Vec2(700.f, 500.f*fac), 2.f);
		sprite->RunAction(action);
	}

	// Rotate By
	for (int i = 0; i < 2; i++)
	{
		Pim::Sprite *sprite = new Pim::Sprite("lighttiles.png");
		sprite->position = Pim::Vec2(400.f, 50.f + 500.f * i);
		AddChild(sprite);

		Pim::RotateByAction *action = new Pim::RotateByAction(-720.f, 2.f);
		sprite->RunAction(action);
	}

	// Tint
	for (int x = -1; x <= 1; x += 2)
	{
		for (int y = -1; y <= 1; y += 2)
		{
			Pim::Sprite *sprite = new Pim::Sprite("lighttiles.png");
			sprite->position = Pim::Vec2(400.f + 100.f*x, 300.f + 100.f*y);
			AddChild(sprite);

			Pim::Color color(
				0.5f + 0.25f*x,
				0.5f - 0.25f*y,
				1.f,
				1.f
				);

			Pim::TintAction *action = new Pim::TintAction(color, 2.f);
			sprite->RunAction(action);
		}
	}

	// Action Queue
	for (int i = -1; i <= 1; i += 2)
	{
		Pim::DelayAction *delay = new Pim::DelayAction(0.5f);
		Pim::MoveByAction *a0 = new Pim::MoveByAction(Pim::Vec2(10.f*i, 10.f), 0.25f);
		Pim::MoveByAction *a1 = new Pim::MoveByAction(Pim::Vec2(10.f, 10.f*i), 0.25f);
		Pim::TintAction *a2 = new Pim::TintAction(Pim::Color(1.f, 1.f, 1.f, 0.f), 0.25f);
		Pim::TintAction *a3 = new Pim::TintAction(Pim::Color(1.f, 1.f, 1.f, 1.f), 0.25f);
		Pim::MoveToAction *a4 = new Pim::MoveToAction(Pim::Vec2(50.f, 300.f+250.f*i), 0.5f);

		Pim::TintAction *a5 = new Pim::TintAction(Pim::Color(0.f, 0.f, 0.f, 1.f), 0.5f);
		Pim::TintAction *a6 = new Pim::TintAction(Pim::Color(1.f, 1.f, 1.f, 1.f), 0.5f);
		Pim::ActionQueueRepeat *aqr = new Pim::ActionQueueRepeat(1,2,a5,a6);
		aqr->infinite = true;

		Pim::ActionQueue *aq = new Pim::ActionQueue(7, delay, a0, a1, a2, a3, a4, aqr);

		Pim::Sprite *sprite = new Pim::Sprite("lighttiles.png");
		sprite->position = Pim::Vec2(400.f + 200.f*i, 100.f);
		AddChild(sprite);

		sprite->RunActionQueue(aq);
	}

	// ActionQueueRepeat
	for (int i = -1; i <= 1; i += 2)
	{
		Pim::TintAction		*a0 = new Pim::TintAction(Pim::Color(1.f, 0.f, 0.f, 1.f), 0.25f);
		Pim::RotateByAction *a1 = new Pim::RotateByAction(180.f, 0.5f); 
		Pim::ScaleByAction	*a2 = new Pim::ScaleByAction(Pim::Vec2(1.f, 1.f), 0.25f);
		Pim::ScaleByAction	*a3 = new Pim::ScaleByAction(Pim::Vec2(-1.f, -1.f), 0.25f);
		Pim::MoveToAction	*a4 = new Pim::MoveToAction(Pim::Vec2(400.f, 300.f), 0.25f);
		Pim::MoveByAction	*a5 = new Pim::MoveByAction(Pim::Vec2(-300.f*i, 0.f), 0.25f);
		Pim::TintAction		*a6 = new Pim::TintAction(Pim::Color(1.f, 1.f, 1.f, 1.f), 0.25f);
		Pim::ActionQueueRepeat *aq = new Pim::ActionQueueRepeat(2, 7, a0,a1,a2,a3,a4,a5,a6);
		aq->infinite = true;

		Pim::Sprite *sprite = new Pim::Sprite("lighttiles.png");
		sprite->position = Pim::Vec2(400.f + 300.f*i, 300.f);
		AddChild(sprite);

		sprite->RunActionQueue(aq);
	}

	shader = Pim::ShaderManager::AddShader(
		"uniform sampler2D tex;												\
		uniform float time;													\
		void main(void)														\
		{																	\
			vec2 c = gl_TexCoord[0];										\
			float m = (sin(c.x*100.0)+1.0)/100.0;							\
			c.x += m;														\
			gl_FragColor = texture2D(tex, c);								\
		}",
		"void main(){														\
			gl_Position=gl_ModelViewProjectionMatrix*gl_Vertex;				\
			gl_TexCoord[0] = gl_MultiTexCoord0;								\
		}",
		"myshad");
	if (shader)
	{
		shader->SetUniform1i("tex", 0);
		SetShader(shader);
	}
}

/*
=====================
ActionLayer::Update
=====================
*/
void ActionLayer::Update(float dt)
{
	time += dt;
	shader->SetUniform1f("time", time);
}

/*
=====================
ActionLayer::ActionCompleted
=====================
*/
void ActionLayer::ActionCompleted(Pim::BaseAction *act)
{
	
}