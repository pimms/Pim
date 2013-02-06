#include "PimRenderWindowWIN.h"
#include "PimGameControl.h"
#include "PimLightingSystem.h"

namespace Pim {

	

	/*
	====================
	RenderWindowWIN::RenderWindowWIN
	====================
	*/
	RenderWindowWIN::RenderWindowWIN(WinStyle::CreationData &data)
	: RenderWindowBase(data) {
		hWnd		= NULL;
		hInstance	= NULL;
	}

	/*
	====================
	RenderWindowWIN::~RenderWindowWIN
	====================
	*/
	RenderWindowWIN::~RenderWindowWIN() {
		KillWindow();
	}

	
}