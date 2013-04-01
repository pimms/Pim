#include "PimInternal.h"

#include "PimVec2.h"
#include "PimLightingSystem.h"
#include "PimLightDef.h"
#include "PimRenderWindow.h"
#include "PimShaderManager.h"
#include "PimGameControl.h"
#include "PimGameNode.h"
#include "PimLayer.h"
#include "PimSprite.h"
#include "PimPolygonShape.h"
#include "PimAssert.h"

#include "PimLightingSystemShaders.h"


namespace Pim {
	int LightingSystem::numSystemsCreated = 0;

	/*
	=====================
	LightingSystem::CreateSmoothLightTexture
	=====================
	*/
	void LightingSystem::CreateSmoothLightTexture(LightDef *lightDef, bool preload) {
		SmoothLightDef *lDef = (SmoothLightDef*)lightDef;

		RenderTexture *rt = new RenderTexture(Pim::Vec2(lDef->radius*2.f, lDef->radius*2.f));
		rt->BindFBO();
		rt->Clear();
		rt->retainTexture = true;

		glDisable(GL_TEXTURE_2D);

		// Render the light texture
		Color ic = lDef->innerColor;
		Color oc = lDef->outerColor;

		glTranslatef((float)lDef->radius, (float)lDef->radius, 0.f);

		// The outer circle
		const float step = 6.283f/100.f;
		for (int i=0; i<lDef->innerPasses; i++) {
			glBegin(GL_TRIANGLE_FAN);
				glColor4d(ic.r, ic.g, ic.b, ic.a);
				glVertex2f(0.f, 0.f);
				glColor4f(oc.r, oc.g, oc.b, oc.a);
				for (float a=0.f; a+step<6.283f; a+=step) {
					glVertex2f(	cosf(a)*lDef->radius,
								sinf(a)*lDef->radius);
				}
				glVertex2f(	cosf(0.f)*lDef->radius,
							sinf(0.f)*lDef->radius);
			glEnd();
		}

		glEnable(GL_TEXTURE_2D);

		lDef->lTex = rt->GetTex();
		if (preload) {
			lightDef->isPreloaded = true;
		}

		rt->UnbindFBO();
		delete rt;
	}

	/*
	=====================
	LightingSystem::CreateFlatLightTexture
	=====================
	*/
	void LightingSystem::CreateFlatLightTexture(LightDef *lightDef, bool preload) {
		FlatLightDef *lDef = (FlatLightDef*)lightDef;

		if (lDef->falloff < 0.f) {
			lDef->falloff = 0.f;
		}

		float totalRadius = lDef->radius + (lDef->radius * lDef->falloff);

		RenderTexture *rt = new RenderTexture(Pim::Vec2(totalRadius*2,totalRadius*2));
		rt->BindFBO();
		rt->Clear();
		rt->retainTexture = true;

		// Render the light texture
		Color ic = lDef->innerColor;
		Color oc = lDef->outerColor;

		glDisable(GL_TEXTURE_2D);
		glTranslatef(totalRadius, totalRadius, 0.f);

		// The inner circle
		const float step = 6.283f/100.f;
		glBegin(GL_TRIANGLE_FAN);
			glColor4f(ic.r, ic.g, ic.b, ic.a);
			glVertex2f(0.f, 0.f);
			for (float a=0.f; a+step<6.283f; a+=step) {
				glVertex2f(cosf(a)*lDef->radius, sinf(a)*lDef->radius);
			}
			glVertex2f(cosf(0.f)*lDef->radius, sinf(0.f)*lDef->radius);
		glEnd();

		if (lDef->falloff != 0.f) {
			// The falloff
			glBegin(GL_TRIANGLE_STRIP);
				for (float a=0.f; a+step<6.283f; a+=step) {
					// Use the inner color..
					glColor4f(ic.r, ic.g, ic.b, ic.a);
					glVertex2f(cosf(a)*lDef->radius, sinf(a)*lDef->radius);

					// Then the outer color
					glColor4f(oc.r, oc.g, oc.b, oc.a);
					glVertex2f(cosf(a)*totalRadius, sinf(a)*totalRadius);
				}

				glColor4f(ic.r, ic.g, ic.b, ic.a);
				glVertex2f(cosf(0.f)*lDef->radius, sinf(0.f)*lDef->radius);

				glColor4f(oc.r, oc.g, oc.b, 0.f);
				glVertex2f(cosf(0.f)*totalRadius, sinf(0.f)*totalRadius);
			glEnd();
		}

		glEnable(GL_TEXTURE_2D);

		// Update the radius value
		lDef->radius = totalRadius;

		lDef->lTex = rt->GetTex();
		if (preload) {
			lightDef->isPreloaded = true;
		}

		rt->UnbindFBO();
		delete rt;
	}



	/*
	=====================
	LightingSystem::LightingSystem
	=====================
	*/
	LightingSystem::LightingSystem(Layer *par, Vec2 pResolution) {
		number			= numSystemsCreated++;

		parent			= par;
		castShadow		= true;
		dbgDrawNormal   = false;
		hqShadow		= false;
		resolution		= pResolution;
		
		shaderLightTex	= NULL;
		shaderGauss		= NULL;
		shaderNormalMap = NULL;

		mainRT = new RenderTexture(resolution, true);
		gaussRT = new RenderTexture(resolution);

		LoadShaders();
	}

	/*
	=====================
	LightingSystem::~LightingSystem
	=====================
	*/
	LightingSystem::~LightingSystem() {
		for (auto it=lights.begin(); it!=lights.end(); it++) {
			delete it->second;
		}

		for (auto it=preloadTex.begin(); it!=preloadTex.end(); it++) {
			if (it->second) {
				glDeleteTextures(1, &it->second);
			}
		}

		if (shaderLightTex) {
			ShaderManager::RemoveShader(shaderLightTex);
		}

		if (shaderGauss) {
			ShaderManager::RemoveShader(shaderGauss);
		}

		if (shaderNormalMap) {
			ShaderManager::RemoveShader(shaderNormalMap);
		}

		delete mainRT;
		delete gaussRT;
	}


	/*
	=====================
	LightingSystem::GetNormalMapShader
	=====================
	*/
	Shader* LightingSystem::GetNormalMapShader() {
		return shaderNormalMap;
	}


	/*
	=====================
	LightingSystem::SetUnlitColor
	=====================
	*/
	void LightingSystem::SetUnlitColor(const Color c) {
		color = c;
		shaderLightTex->SetUniform4f("ulcolor", c.r, c.g, c.b, c.a);
		shaderGauss->SetUniform4f("ulcolor", c.r, c.g, c.b, c.a);
	}

	/*
	=====================
	LightingSystem::SetLightAlpha
	=====================
	*/
	void LightingSystem::SetLightAlpha(float a) {
		if (a > 1.f ) a = 1.f;
		if (a < 0.f) a = 0.f;

		a = 1.f - a;
		shaderLightTex->SetUniform1f("lalpha", a);
		shaderGauss->SetUniform1f("lalpha", a);
	}

	/*
	=====================
	LightingSystem::SetCastShadows
	=====================
	*/
	void LightingSystem::SetCastShadows(bool flag) {
		castShadow = flag;
	}

	/*
	=====================
	LightingSystem::SetDebugDrawShadowShapes
	=====================
	*/
	void LightingSystem::SetDebugDrawShadowShapes(bool flag) {
		dbgDrawNormal = flag;
	}


	/*
	=====================
	LightingSystem::AddLight
	=====================
	*/
	void LightingSystem::AddLight(GameNode *node, LightDef *lDef) {
		if (lights.count(node)) {
			delete lights[node];
		}

		lights[node] = lDef;

		if (lDef->lightType == 0) {
			CreateFlatLightTexture(lDef);
		} else if (lDef->lightType == 1) {
			CreateSmoothLightTexture(lDef);
		} else if (lDef->lightType != 2) {
			PimAssert(0, "Error: invalid light type!");
		}
	}

	/*
	=====================
	LightingSystem::AddShadowCaster
	=====================
	*/
	void LightingSystem::AddShadowCaster(GameNode *caster) {
		PimAssert(
			caster->GetShadowShape() != NULL, 
			"Error: Shadow Caster must have a defined shadow shape"
		);

		casters.push_back(caster);
	}

	/*
	=====================
	LightingSystem::RemoveLight
	=====================
	*/
	void LightingSystem::RemoveLight(GameNode *light) {
		if (lights.count(light)) {
			delete lights[light];
			lights.erase(light);

			/* Remove it if it is a normalmap affecting light */
			for (unsigned i=0; i<normalLights.size(); i++) {
				if (normalLights[i] == light) {
					normalLights.erase(normalLights.begin() + i);
					i--;
				}
			}
		}
	}

	/*
	=====================
	LightingSystem::RemoveShadowCaster
	=====================
	*/
	void LightingSystem::RemoveShadowCaster(GameNode *caster) {
		for (unsigned i=0; i<casters.size(); i++) {
			if (casters[i] == caster) {
				casters.erase(casters.begin() + i--);
			}
		}
	}

	/*
	==================
	LightingSystem::SetNormalLighting
	==================
	*/
	bool LightingSystem::SetNormalLighting(GameNode *light, bool flag) {
		/* No more room */
		if (normalLights.size() >= 10 && flag) {
			return false;
		}

		/* The light is already flagged for normal lighting */
		for (unsigned i=0; i<normalLights.size(); i++) {
			if (light == normalLights[i]) {
				if (flag) {
					return false;
				} else {
					normalLights.erase(normalLights.begin() + i);
					return true;
				}
			}
		}

		/* If "flag", make sure that the passed light _really_ is a light */
		if (flag && lights.count(light)) {
			normalLights.push_back(light);
			return true;
		} else {
			return false;
		}
	}


	/*
	=====================
	LightingSystem::PreloadTexture
	=====================
	*/
	void LightingSystem::PreloadTexture(LightDef *ld, const string identifier) {
		if (!identifier.length() || preloadTex.count(identifier)) {
			PimAssert(0, "Error: invalid identifying string!");
		}

		if (ld->lightType == 0) {
			CreateFlatLightTexture(ld, true);
		} else if (ld->lightType == 1) {
			CreateSmoothLightTexture(ld, true);
		} else {
			PimAssert(0, "Error: invalid light type!");
		}

		preloadTex[identifier] = ld->lTex;
	}

	/*
	=====================
	LightingSystem::UsePreloadedTexture
	=====================
	*/
	bool LightingSystem::UsePreloadedTexture(LightDef *ld, const string identifier) {
		if (preloadTex.count(identifier)) {
			ld->lTex = preloadTex[identifier];
			return true;
		}
		return false;
	}

	/*
	=====================
	LightingSystem::DeletePreloadedTexture
	=====================
	*/
	void LightingSystem::DeletePreloadedTexture(const string identifier) {
		if (preloadTex.count(identifier)) {
			if (preloadTex[identifier]) {
				glDeleteTextures(1, &preloadTex[identifier]);
				preloadTex.erase(identifier);
			}
		}
	}


	/*
	=====================
	LightingSystem::LoadShaders
	=====================
	*/
	void LightingSystem::LoadShaders() {
		stringstream name;
		name << number;

		/* Low quality, default shader */
		if (!shaderLightTex) {
			shaderLightTex = ShaderManager::AddShader(
					PIM_LS_LOWQ_FRAG, PIM_BAREBONES_VERT, "ltMgr" + name.str()
			);

			shaderLightTex->SetUniform1i("texSrc", 0);
			shaderLightTex->SetUniform1f("lalpha", 1.f);
			shaderLightTex->SetUniform4f("ulcolor", 0.f, 0.f, 0.f, 1.f);
		}

		/* High quality gauss shader */
		if (!shaderGauss) {
			shaderGauss = ShaderManager::AddShader(
					PIM_LS_HIGHQ_FRAG, PIM_BAREBONES_VERT, "ltMgrGauss" + name.str()
			);

			shaderGauss->SetUniform1i("blurSampler", 0);
			shaderGauss->SetUniform1f("blurSize", 1.f/resolution.y);
			shaderGauss->SetUniform1f("sigma", 5.f);
			shaderGauss->SetUniform1f("lalpha", 1.f);
			shaderGauss->SetUniform1f("pixels", 5.f);
			shaderGauss->SetUniform4f("ulcolor", 0.f, 0.f, 0.f, 1.f);
		}

		/* Normal map shader */
		if (!shaderNormalMap) {
			shaderNormalMap = ShaderManager::AddShader(
				PIM_LS_NORMALMAP_FRAG, PIM_BAREBONES_VERT, "ltMgrNormal" + name.str()
			);

			shaderNormalMap->SetUniform1i("tex0", 0);
			shaderNormalMap->SetUniform1i("tex1", 1);
		}
	}

	/*
	==================
	LightingSystem::UpdateShaderUniforms
	==================
	*/
	void LightingSystem::UpdateShaderUniforms() {
		GLfloat pos[20] = { 0.f };
		GLfloat rad[10] = { 0.f };

		for (unsigned i=0; i<normalLights.size() && i<10; i++) {
			GameNode *light = normalLights[i];

			pos[i*2 + 0] = light->position.x;
			pos[i*2 + 1] = light->position.y;

			if (lights.count(light)) {
				rad[i] = lights[light]->radius;
			}
		}

		shaderNormalMap->EnableShader();

		glUniform1i(shaderNormalMap->GetUniformLocation("numLights"), (int)normalLights.size());
		glUniform2fv(shaderNormalMap->GetUniformLocation("lpos"), 10, pos);
		glUniform1fv(shaderNormalMap->GetUniformLocation("lrad"), 10, rad);

		shaderNormalMap->DisableShader();
	}

	/*
	=====================
	LightingSystem::RenderLightTexture
	=====================
	*/
	void LightingSystem::RenderLightTexture() {
		// The Window Dimensions
		Vec2 wd = GameControl::GetSingleton()->GetRenderWindow()->ortho;

		// Prepare the mainRT
		mainRT->BindFBO();
		mainRT->Clear(GL_STENCIL_BUFFER_BIT);

		// Render the lights and shadows onto the mainRT
		RenderLights();

		glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
		glDisable(GL_TEXTURE_2D);
		glColor4f(color.r, color.g, color.b, color.a);

		// Render the unlit color over the mainRT in blank areas
		glBegin(GL_QUADS);
			glVertex2f(0.f, 0.f);
			glVertex2f(resolution.x, 0.f);
			glVertex2f(resolution.x, resolution.y);
			glVertex2f(0.f, resolution.y);
		glEnd();
		glEnable(GL_TEXTURE_2D);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4ub(255,255,255,255);

		// Unbind the FBO
		mainRT->UnbindFBO();

		if (hqShadow) {
			GaussPass();
			gaussRT->BindTex();
		} else {
			glUseProgram(shaderLightTex->GetProgram());
			mainRT->BindTex();
		}

		glPushMatrix();				// Render to main FBO
		glLoadIdentity();

		glBegin(GL_QUADS);
			glTexCoord2i(0,0); glVertex2f(0.f, 0.f);
			glTexCoord2i(1,0); glVertex2f(wd.x, 0.f);
			glTexCoord2i(1,1); glVertex2f(wd.x, wd.y);
			glTexCoord2i(0,1); glVertex2f(0.f, wd.y);
		glEnd();

		glUseProgram(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();				// Render to main FBO
	}

	/*
	=====================
	LightingSystem::GaussPass
	=====================
	*/
	void LightingSystem::GaussPass() {
		shaderGauss->SetUniform2f("direction", 0.f, 1.f);
		glUseProgram(shaderGauss->GetProgram());

		gaussRT->BindFBO();
		gaussRT->Clear();
		mainRT->BindTex();

		glBegin(GL_QUADS);
			glTexCoord2i(0,0); glVertex2f(0.f, 0.f);
			glTexCoord2i(1,0); glVertex2f(resolution.x, 0.f);
			glTexCoord2i(1,1); glVertex2f(resolution.x, resolution.y);
			glTexCoord2i(0,1); glVertex2f(0.f, resolution.y);
		glEnd();

		gaussRT->UnbindFBO();

		glUseProgram(0);
		shaderGauss->SetUniform2f("direction", 1.f, 0.f);

		glUseProgram(shaderGauss->GetProgram());
	}

	/*
	=====================
	LightingSystem::RenderLights
	=====================
	*/
	void LightingSystem::RenderLights() {
		Vec2 renres = GameControl::GetSingleton()->GetCreationData().renderResolution;
		Vec2 coord = GameControl::GetSingleton()->GetCreationData().coordinateSystem;
		Vec2 lightScale		= renres / resolution;		// The scale of the light texture
		Vec2 posScale		= resolution / coord;		// The position (coord) scale
		Vec2 lineScale		= coord / renres;			// The shadow line position scale

		glEnable(GL_STENCIL_TEST);

		glPushMatrix();						// Layer position & scale
		glScalef(posScale.x, posScale.y, 1.f);
		glTranslatef(parent->position.x, parent->position.y, 0.f);
		glScalef(parent->scale.x, parent->scale.y, 1.f);

		for (auto it=lights.begin(); it!=lights.end(); it++) {
			float r = it->second->radius;
			Vec2 p = (it->first->GetLayerPosition() + it->second->position);

			if (castShadow && it->second->castShadows) {
				RenderShadows(it->second, it->first, p, lineScale);
			}

			glBindTexture(GL_TEXTURE_2D, it->second->lTex);
			glPushMatrix();					// Light texture

			//p *= parent->scale;
			glTranslatef(p.x, p.y, 0.f);

			glStencilFunc(GL_NOTEQUAL, 0x1, 0x1);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

			glColor4f(1.f, 1.f, 1.f, 0.2f);

			glScalef(lightScale.x, lightScale.y, 1.f);

			glBegin(GL_QUADS);
				glTexCoord2i(0,0); glVertex2f(-r,-r);
				glTexCoord2i(1,0); glVertex2f(r,-r);
				glTexCoord2i(1,1); glVertex2f(r,r);
				glTexCoord2i(0,1); glVertex2f(-r,r);
			glEnd();

			glPopMatrix();					// Light texture
		}

		glPopMatrix();						// Layer position & scale

		glDisable(GL_STENCIL_TEST);
	}

	/*
	=====================
	LightingSystem::RenderShadows
	=====================
	*/
	void LightingSystem::RenderShadows(LightDef *ld, GameNode *light, 
										const Vec2 &pos, const Vec2 &sc) {
		// PARAMETERS:
		//	ld:			The light definition struct
		//	light:		The game node, acting as a light
		//	pos:		The position of the light relative to it's parent
		//	sc:			The scale (lightingSys.resolution / renderResolution)

		float r = ld->radius + ld->radius*ld->falloff;
		vector<Line*> castLines;


		glPushMatrix();				// Shadows
		glTranslatef(pos.x, pos.y, 0.f);

		glClear(GL_STENCIL_BUFFER_BIT);

		glStencilFunc(GL_NEVER, 0x1, 0x0);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

		glDisable(GL_TEXTURE_2D);
		glColor4f(color.r, color.g, color.b, 1.f);

		#ifdef _DEBUG	
		if (dbgDrawNormal) {
			glLineWidth(2.f);
			glColor4f(0.f, 1.f, 0.f, 1.f);
			glDisable(GL_STENCIL_TEST);
			glBegin(GL_LINES);
		}
		#endif /* _DEBUG */

		for (unsigned int i=0; i<casters.size(); i++) {
			auto lines = casters[i]->shadowShape->lines;
			{
				for (unsigned int i=0; i<lines.size(); i++) {
					if ((pos-lines[i]->GetMid(sc)).Length() <= r ||
							(pos-lines[i]->GetP1(sc)).Length()  <= r ||
							(pos-lines[i]->GetP2(sc)).Length()  <= r) {
						if (lines[i]->IsFacing(pos, sc)) {
							castLines.push_back(lines[i]);

							#ifdef _DEBUG
							if (dbgDrawNormal) {
								glVertex2f(lines[i]->GetP1(sc).x-pos.x, 
											lines[i]->GetP1(sc).y-pos.y);
								glVertex2f(lines[i]->GetP2(sc).x-pos.x, 
											lines[i]->GetP2(sc).y-pos.y);

								glVertex2f(lines[i]->GetMid(sc).x-pos.x, 
											lines[i]->GetMid(sc).y-pos.y);
								glVertex2f(lines[i]->GetNormalEnd(sc).x-pos.x,
										   lines[i]->GetNormalEnd(sc).y-pos.y);
							}
							#endif /* _DEBUG */
						}
					}
				}
			}
		}

		#ifdef _DEBUG
		if (dbgDrawNormal) {
			glEnd();
			glEnable(GL_STENCIL_TEST);
			glColor4f(color.r, color.g, color.b, 1.f);
			glLineWidth(1.f);
		}
		#endif /* _DEBUG */

		glBegin(GL_QUADS);

		for (unsigned int i=0; i<castLines.size(); i++) {
			Vec2  v1 = (pos-castLines[i]->GetP1(sc)),
				  v2 = (pos-castLines[i]->GetP2(sc));
			float a1 = v1.AngleBetween(Vec2(1.f,0.f)),
				  a2 = v2.AngleBetween(Vec2(1.f,0.f));

			glVertex2f(-v1.x, -v1.y);
			glVertex2f(-v2.x, -v2.y);

			v1 += Vec2( cosf(a1*((float)M_PI/180.f)),
						sinf(a1*((float)M_PI/180.f))) * (float)r * 100.f;
			v2 += Vec2( cosf(a2*((float)M_PI/180.f)),
						sinf(a2*((float)M_PI/180.f))) * (float)r * 100.f;

			glVertex2f(-v2.x, -v2.y);
			glVertex2f(-v1.x, -v1.y);

		}

		glEnd();

		glColor4f(1.f, 1.f, 1.f, 1.f);
		glEnable(GL_TEXTURE_2D);

		glPopMatrix();				// Shadows
	}
}