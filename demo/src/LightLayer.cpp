#include "LightLayer.h"
#include "HUDLayer.h"

#include <iostream>
#include <string>

#include <functional>

#define R01 ((float)rand()/(float)RAND_MAX)
#define RLN (200 + R01*50);

/*
==================
LightLayer::~LightLayer
==================
*/
LightLayer::~LightLayer()
{
}


/*
==================
LightLayer::LoadResources
==================
*/
void LightLayer::LoadResources()
{
	ListenInput();
	ListenFrame();

	srand((unsigned int)time(0));

	poly = false;
	norm = false;

	BindKeys();
	LoadLightingSystem();
	LoadSprites();

	color = Pim::Color(1.f, 1.f, 1.f, 1.f);

	HUDLayer *hud = new HUDLayer;
	hud->LoadResources();
	AddChild(hud);

	//scale = Pim::Vec2(2.f, 2.f);
}

/*
==================
LightLayer::LoadSprites
==================
*/
void LightLayer::LoadSprites()
{
	// Create the normal map
	Pim::NormalMap *normal = new Pim::NormalMap("brown.png", "normalmap.png");
	normal->position = Pim::Vec2(400.f, 300.f);
	normal->scale *= 0.7f;
	AddChild(normal);
}

/*
==================
LightLayer::LoadLightingSystem
==================
*/
void LightLayer::LoadLightingSystem()
{
	CreateLightingSystem(Pim::Vec2(800.f, 600.f));
	SetLightingUnlitColor(Pim::Color(0.f, 0.f, 0.f, 1.f));
	SetLightAlpha(0.f);
	SetShadowcasterDebugDraw(norm);
	SetSmoothShadows(true);

	Pim::LightDef *ld1 = new Pim::FlatLightDef;
	ld1->innerColor = Pim::Color(1.f, 1.f, 1.f, 1.f);
	ld1->outerColor = Pim::Color(1.f, 1.f, 1.f, 0.f);
	ld1->falloff = 1.f;
	ld1->radius = 400.f;
	PreloadLightTexture(ld1, "preload");

	Pim::PreloadLightDef *pld = new Pim::PreloadLightDef;
	pld->radius = 400.f;

	light = new Pim::GameNode;
	light->position = Pim::Vec2(170.f, 150.f);
	AddChild(light);
	AddLight(light, pld, "preload");
	lightSys->SetNormalLighting(light, true);
}


/*
==================
LightLayer::BindKeys
==================
*/
void LightLayer::BindKeys()
{
	Pim::Input::GetSingleton()->BindKey("poly", Pim::KeyEvent::K_Z);
	Pim::Input::GetSingleton()->BindKey("norm", Pim::KeyEvent::K_X);

	Pim::Input::GetSingleton()->BindKey("borderless", Pim::KeyEvent::K_F5);
	Pim::Input::GetSingleton()->BindKey("windowed", Pim::KeyEvent::K_F6);
}


/*
==================
LightLayer::OnKeyEvent
==================
*/
void LightLayer::OnKeyEvent(Pim::KeyEvent &evt)
{
	// Debug drawing
	if (evt.IsKeyFresh("poly")) {
		poly = !poly;
		for (unsigned i=0; i<casters.size(); i++) {
			casters[i]->SetShadowShapeDebugDraw(poly);
		}
	}	
	if (evt.IsKeyFresh("norm")) {
		norm = !norm;
		SetShadowcasterDebugDraw(norm);
	}

	// Movement of the layer
	if (evt.IsKeyDown(Pim::KeyEvent::K_D)) {
		position.x -= 3.f;
	}
	if (evt.IsKeyDown(Pim::KeyEvent::K_A)) {
		position.x += 3.f;
	}
	if (evt.IsKeyDown(Pim::KeyEvent::K_S)) {
		position.y += 3.f;
	}
	if (evt.IsKeyDown(Pim::KeyEvent::K_W)) {
		position.y -= 3.f;
	}

	// Changing of the window style
	if (evt.IsKeyFresh("borderless")) {
		Pim::GameControl::GetSingleton()->SetWindowStyle(Pim::WinStyle::BORDERLESS_WINDOWED);
	}
	if (evt.IsKeyFresh("windowed")) {
		Pim::GameControl::GetSingleton()->SetWindowStyle(Pim::WinStyle::WINDOWED);
	}
}

/*
==================
LightLayer::OnMouseEvent
==================
*/
void LightLayer::OnMouseEvent(Pim::MouseEvent &evt)
{
	mousePos = evt.GetPosition();

	if (evt.IsKeyFresh(Pim::MouseEvent::MBTN_LEFT))
	{
		//removeChild(light, true);

		Pim::PreloadLightDef *pld = new Pim::PreloadLightDef;
		pld->radius = 400.f;

		light = new Pim::GameNode;
		AddChild(light);
		AddLight(light, pld, "preload");
		lightSys->SetNormalLighting(light, true);
	}

	if (evt.IsKeyFresh(Pim::MouseEvent::MBTN_RIGHT))
	{
		Pim::GameNode *node = new Pim::GameNode;
		node->position = evt.GetPosition() - position;
		AddChild(node);

		Pim::Vec2 verts[16];
		float rad = 0.f;
		for (int i=0; i<16; i++)
		{
			rad += (M_PI*2.f)/16.f;
			float r = 50.f;// rand() % 40+30;
			verts[i] = Pim::Vec2(r*cosf(rad), r*sinf(rad));
		}

		node->SetShadowShape(verts, 16);

		AddShadowCaster(node);
		//casters.push_back(node);
	}
}

/*
==================
LightLayer::Update
==================
*/
void LightLayer::Update(float dt)
{
	light->position = mousePos - position;
}