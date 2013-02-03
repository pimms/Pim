#include "PimLightDef.h"
#include "PimLightingSystem.h"

namespace Pim {
	/*
	=====================
	LightDef::LightDef
	=====================
	*/
	LightDef::LightDef() {
		innerColor		= Color(1.f, 1.f, 1.f, 1.f);
		outerColor		= Color(0.2f, 0.2f, 0.2f, 0.0f);
		castShadows		= true;
		radius			= 128.f;
		lTex			= 0;
		falloff			= 0.2f;
		lightType		= -1;
		isPreloaded		= false;
	}

	/*
	=====================
	LightDef::~LightDef
	=====================
	*/
	LightDef::~LightDef() {
		if (lTex && !isPreloaded) {
			glDeleteTextures(1, &lTex);
		}
	}

	/*
	=====================
	LightDef::GetTexID
	=====================
	*/
	GLuint LightDef::GetTexID() const {
		return lTex;
	}

	/*
	=====================
	FlatLightDef::FlatLightDef
	=====================
	*/
	FlatLightDef::FlatLightDef() {
		lightType = 0;
	}

	/*
	=====================
	SmoothLightDef::SmoothLightDef
	=====================
	*/
	SmoothLightDef::SmoothLightDef() {
		lightType = 1;
		innerPasses = 3;
	}

	/*
	=====================
	PreloadLightDef::PreloadLightDef
	=====================
	*/
	PreloadLightDef::PreloadLightDef() {
		isPreloaded = true;
		lightType	= 2;
	}
}