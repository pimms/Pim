#include "PimInternal.h"

#include "PimLightDef.h"
#include "PimLayer.h"
#include "PimGameNode.h"
#include "PimVec2.h"
#include "PimGameControl.h"
#include "PimAssert.h"
#include "PimScene.h"
#include "PimRenderTexture.h"
#include "PimShaderManager.h"
#include "PimRenderWindow.h"

namespace Pim {
	/*
	=====================
	Layer::Layer
	=====================
	*/
	Layer::Layer(void) {
		color		= Color(1.f, 1.f, 1.f, 1.f);
		immovable	= false;
		scale		= Vec2(1.f, 1.f);
		lightSys	= NULL;
		parentScene = NULL;
		shader		= NULL;
		rt			= NULL;
	}

	/*
	=====================
	Layer::Layer
	=====================
	*/
	Layer::~Layer(void) {
		DestroyLightingSystem();

		if (rt) {
			delete rt;
		}
	}

	/*
	=====================
	Layer::Layer
	=====================
	*/
	Scene* Layer::GetParentScene() const {
		if (parent) {
			return parent->GetParentScene();
		} else {
			return parentScene;
		}
	}

	/*
	=====================
	Layer::Layer
	=====================
	*/
	Layer* Layer::GetParentLayer() {
		return this;
	}

	/*
	=====================
	Layer::Layer
	=====================
	*/
	Vec2 Layer::GetWorldPosition() const {
		if (immovable) {
			return position;
		}

		if (parent) {
			return position + parent->GetWorldPosition();
		}

		return position;
	}

	/*
	=====================
	Layer::Layer
	=====================
	*/
	float Layer::GetWorldRotation() const {
		if (parent) {
			return rotation + parent->GetWorldRotation();
		}

		return rotation;
	}

	/*
	=====================
	Layer::Layer
	=====================
	*/
	Vec2 Layer::GetLayerPosition() const {
		return Vec2(0.f, 0.f);
	}

	/*
	=====================
	Layer::Layer
	=====================
	*/
	void Layer::SetImmovableLayer(bool immov) {
		immovable = immov;
	}

	/*
	=====================
	Layer::Layer
	=====================
	*/
	void Layer::Draw() {
		PrepareRT();

		glPushMatrix();

		if (immovable) {
			glLoadIdentity();
		}

		// Update position
		Vec2 fac = GameControl::GetSingleton()->GetCoordinateFactor();

		glTranslatef(position.x / fac.x, position.y / fac.y, 0.f);
		glRotatef(rotation, 0.f, 0.f, 1.f);

		fac = GameControl::GetSingleton()->GetWindowScale();
		glScalef(scale.x, scale.y, 1.f);

		OrderChildren();

		for (unsigned int i=0; i<children.size(); i++) {
			children[i]->Draw();
		}

		if (lightSys) {
			lightSys->RenderLightTexture();
		}

		glPopMatrix();

		RenderRT();
	}

	/*
	=====================
	Layer::Layer
	=====================
	*/
	void Layer::SetZOrder(const int z) {
		if (parent) {
			parent->dirtyZOrder = true;
		} else if (parentScene) {
			parentScene->dirtyZOrder = true;
		}

		zOrder = z;
	}

	/*
	=====================
	Layer::Layer
	=====================
	*/
	void Layer::SetShader(Shader *s) {
		shader = s;

		if (s && !rt) {
				curRTRes = GameControl::GetSingleton()->GetRenderWindow()->GetOrtho();
				rt = new RenderTexture(curRTRes);
		}
	}

	/*
	=====================
	Layer::GetColor
	=====================
	*/
	Color Layer::GetColor() const {
		return color;
	}
	
	// ---------- LIGHTING SYSTEM METHODS ----------


	/*
	=====================
	Layer::Layer
	=====================
	*/
	void Layer::CreateLightingSystem(Vec2 resolution) {
		DestroyLightingSystem();
		lightSys = new LightingSystem(this, resolution);
	}

	/*
	=====================
	Layer::Layer
	=====================
	*/
	void Layer::DestroyLightingSystem() {
		if (lightSys) {
			delete lightSys;
			lightSys = NULL;
		}
	}

	/*
	=====================
	Layer::Layer
	=====================
	*/
	void Layer::AddLight(GameNode *node, LightDef *lDef) {
		PimAssert(node->parent != NULL, "ERROR: Orphan lights are not allowed");

		if (lightSys) {
			lightSys->AddLight(node, lDef);
		}
	}

	/*
	=====================
	Layer::Layer
	
		Add a light using a preloaded texture.
		Define the light texture to use with the identifier string.
		Lights can be preloaded through the PreloadLightDef method.
	=====================
	*/
	bool Layer::AddLight(GameNode *node, PreloadLightDef *pld, const string identifier) {
		if (lightSys) {
			if (!lightSys->UsePreloadedTexture(pld, identifier)) {
				return false;
			}

			AddLight(node, pld);
			return true;
		}

		return false;
	}

	/*
	=====================
	Layer::Layer
	=====================
	*/
	void Layer::RemoveLight(GameNode *node) {
		if (lightSys && lightSys->lights.count(node)) {
			delete lightSys->lights[node];
			lightSys->lights.erase(node);
		}
	}

	/*
	=====================
	Layer::Layer
	=====================
	*/
	void Layer::PreloadLightTexture(LightDef *ld, const string identifier) {
		if (ld) {
			if (lightSys) {
				lightSys->PreloadTexture(ld, identifier);
			}

			delete ld;
		}
	}

	/*
	=====================
	Layer::Layer

		The GameNode must have a ShadowShape prepared before it's added.
		use:	GameNode::SetShadowShape(Vec2[], int)
	=====================
	*/
	void Layer::AddShadowCaster(GameNode *caster) {
		if (lightSys) {
			PimAssert(caster->GetShadowShape() != NULL, 
						"Error: Shadow Caster must have a defined shadow shape");

			lightSys->casters.push_back(caster);
		}
	}

	/*
	=====================
	Layer::Layer
	=====================
	*/
	void Layer::RemoveShadowCaster(GameNode *caster) {
		if (lightSys) {
			for (unsigned int i=0; i<lightSys->casters.size(); i++) {
				if (lightSys->casters[i] == caster) {
					lightSys->casters.erase(lightSys->casters.begin() + i);
				}
			}
		}
	}

	/*
	=====================
	Layer::Layer
	=====================
	*/
	void Layer::SetCastShadows(bool shadows) {
		if (lightSys) {
			lightSys->castShadow = shadows;
		}
	}

	/*
	=====================
	Layer::Layer
	=====================
	*/
	void Layer::SetLightingUnlitColor(const Color color) {
		if (lightSys) {
			lightSys->SetUnlitColor(color);
		}
	}	
	
	/*
	=====================
	Layer::Layer
	=====================
	*/
	void Layer::SetLightAlpha(const float a) {
		if (lightSys) {
			lightSys->SetLightAlpha(a);
		}
	}

	/*
	=====================
	Layer::Layer
	=====================
	*/
	void Layer::SetSmoothShadows(const bool flag) {
		if (lightSys) {
			lightSys->hqShadow = flag;
		}
	}

	/*
	=====================
	Layer::Layer
	=====================
	*/
	void Layer::SetShadowcasterDebugDraw(const bool flag) {
		if (lightSys) {
			lightSys->dbgDrawNormal = flag;
		}
	}

	/*
	=====================
	Layer::Layer
	=====================
	*/
	LightingSystem* Layer::GetLightingSystem() const {
		return lightSys;
	}


	/*
	=====================
	Layer::Layer
	=====================
	*/
	void Layer::PrepareRT() {
		if (shader) {
			if (curRTRes != GameControl::GetSingleton()->GetRenderWindow()->GetOrtho()) {
				delete rt;

				curRTRes = GameControl::GetSingleton()->GetRenderWindow()->GetOrtho();
				rt = new RenderTexture(curRTRes);
			}

			rt->BindFBO();
			rt->Clear();
		} else if (rt) {
			delete rt;
		}
	}

	/*
	=====================
	Layer::Layer
	=====================
	*/
	void Layer::RenderRT() const  {
		if (shader) {
			rt->UnbindFBO();

			glUseProgram(shader->GetProgram());

			rt->BindTex();

			glPushMatrix();
			glLoadIdentity();
			glColor3ub(255,255,255);

			glBegin(GL_QUADS);
				glTexCoord2i(0, 0); glVertex2f(0.f,			0.f);
				glTexCoord2i(1, 0); glVertex2f(curRTRes.x,	0.f);
				glTexCoord2i(1, 1); glVertex2f(curRTRes.x,	curRTRes.y);
				glTexCoord2i(0, 1); glVertex2f(0.f,			curRTRes.y);
			glEnd();

			glPopMatrix();

			glUseProgram(0);
		}
	}
}