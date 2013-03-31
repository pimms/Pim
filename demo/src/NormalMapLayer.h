#pragma once

#include "pim.h"
#include "NormalMap.h"

using namespace Pim;

class NormalMapLayer : public Layer {
public:
	void 			LoadResources();
	void 			OnMouseEvent(MouseEvent &evt);
	
private:
	NormalMap 		*map;
};