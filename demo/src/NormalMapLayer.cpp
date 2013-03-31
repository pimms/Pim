#include "NormalMapLayer.h"


void NormalMapLayer::LoadResources() {
	map = new NormalMap("chester.png", "normal3.png");
	map->position = Vec2(400.f, 300.f);
	AddChild(map);
	
	ListenMouse();
}

void NormalMapLayer::OnMouseEvent(Pim::MouseEvent &evt) {
	if (!map->lpos.size()) {
		map->lpos.push_back(evt.GetPosition());
		map->lpos.push_back(Vec2(400.f, 300.f));
	} else {
		map->lpos[0] = evt.GetPosition();
	}
}