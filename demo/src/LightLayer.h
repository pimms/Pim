#pragma once

#ifdef WIN32
#include "Pim.h"
#elif defined(__APPLE__)
#include <Pim/Pim.h>
#endif

class LightLayer : public Pim::Layer
{
public:
								~LightLayer();
	void						LoadResources();
	void						LoadSprites();
	void						LoadLightingSystem();
	void						BindKeys();

	void						OnKeyEvent(Pim::KeyEvent &evt);
	void						OnMouseEvent(Pim::MouseEvent &evt);
	void						Update(float dt);


	bool						poly;
	bool						norm;
	Pim::GameNode*				light;
	std::vector<Pim::Sprite*>	casters;
	Pim::Vec2					mousePos;
};

