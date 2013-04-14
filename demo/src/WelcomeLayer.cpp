#include "WelcomeLayer.h"

#include <Pim/PimLightingSystemShaders.h>

/*
=====================
WelcomeLayer::LoadResources
=====================
*/
void WelcomeLayer::LoadResources()
{
	Pim::Font *font = new Pim::Font("arial.ttf", 20);

	Pim::Label *lab = new Pim::Label(font);
	lab->SetTextAlignment(Pim::Label::TEXT_CENTER);
	lab->color = Pim::Color(0.f, 0.f, 0.f, 1.f);
	lab->SetLinePadding(5);
	lab->GiveOwnershipOfFont();
	lab->position = Pim::Vec2(400,300);
	lab->SetText("Pim demo application\n\nScroll through the different demonstrations\nwith the buttons.");
	AddChild(lab);

	ListenKeys();
	ListenController();
	
	Pim::Shader *shader = Pim::ShaderManager::GetSingleton()->AddShader(PIM_LS_LOWQ_FRAG, PIM_BAREBONES_VERT, "lolshader");
	if (!shader) {
		printf("HEI DETTE FUNKER IKKE\n");
	}
}

/*
=====================
WelcomeLayer::OnKeyEvent
=====================
*/
void WelcomeLayer::OnKeyEvent(Pim::KeyEvent &evt) {
	if (evt.IsKeyFresh(Pim::KeyEvent::KeyCode::K_A)) {
		printf("A is fresh!\n");
	}

	if (evt.IsKeyDown(Pim::KeyEvent::KeyCode::K_B)) {
		printf("B is down!\n");
	}
}

/*
=====================
WelcomeLayer::OnControllerEvent
=====================
*/
void WelcomeLayer::OnControllerEvent(Pim::ControllerEvent &evt)
{
	Pim::Vec2 l = evt.LeftStick();
	Pim::Vec2 r = evt.RightStick();
}