#include "HUDLayer.h"
#include "LightLayer.h"
#include "DemoScene.h"

class MyButton : public Pim::Button
{
public:

	/*
	==================
	MyButton::MyButton
	==================
	*/
	MyButton(Pim::Sprite* normal, Pim::Sprite* hovered, Pim::Sprite* 
			pressed, Pim::Sprite* deactivated)
			: Pim::Button(normal,hovered,pressed,deactivated)
	{
		
	}

	/*
	==================
	MyButton::MakeNormalCurrent
	==================
	*/
	void MakeNormalCurrent()
	{
		current->color = Pim::Color(1, 1, 1, 1);
	}

	/*
	==================
	MyButton::MakeHoveredCurrent
	==================
	*/
	void MakeHoveredCurrent()
	{
		current->color = Pim::Color(1, 0.5, 0.5, 1);
	}

	/*
	==================
	MyButton::MakePressedCurrent
	==================
	*/
	void MakePressedCurrent()
	{
		current->color = Pim::Color(1,0,0,1);
	}

	/*
	==================
	MyButton::MakeDeactivatedCurrent
	==================
	*/
	void MakeDeactivatedCurrent()
	{
		MakeNormalCurrent();
	}
};

/*
==================
HUDLayer::HUDLayer
==================
*/
HUDLayer::HUDLayer()
{
	immovable = true;
}

/*
==================
HUDLayer::LoadResources
==================
*/
void HUDLayer::LoadResources()
{
	Pim::SpriteBatchNode *batch = new Pim::SpriteBatchNode("hudbutton.png");
	AddChild(batch);

	Pim::Sprite *spr = new Pim::Sprite;
	spr->rect = Pim::Rect(0,0,40,100);
	spr->UseBatchNode(batch);
	
	buttonNext = new MyButton(spr,NULL,NULL,NULL);
	buttonNext->SetCallback((DemoScene*)GetParentScene());
	buttonNext->position = Pim::Vec2(780,300);
	batch->AddChild(buttonNext);

	spr = new Pim::Sprite;
	spr->scale.x = -1;
	spr->rect = Pim::Rect(0,0,40,100);
	spr->UseBatchNode(batch);

	buttonPrev = new MyButton(spr, NULL, NULL, NULL);
	buttonPrev->SetCallback((DemoScene*)GetParentScene());
	buttonPrev->position = Pim::Vec2(20,300);
	batch->AddChild(buttonPrev);
}