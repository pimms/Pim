#include "PimInternal.h"

#include "PimVec2.h"
#include "PimLightingSystem.h"
#include "PimRenderWindow.h"
#include "PimShaderManager.h"
#include "PimGameControl.h"
#include "PimGameNode.h"
#include "PimLayer.h"
#include "PimSprite.h"
#include "PimPolygonShape.h"
#include "PimException.h"


namespace Pim
{

	LightingSystem::LightingSystem(Layer *par, Vec2 pResolution)
	{
		parent			= par;
		castShadow		= true;
		dbgDrawNormal   = false;
		hqShadow		= false;
		resolution		= pResolution;

		rt = new RenderTexture(resolution, true);
		rtGauss = new RenderTexture(resolution);

		loadShaders();
	}
	LightingSystem::~LightingSystem()
	{
		for (auto it=lights.begin(); it!=lights.end(); it++)
		{
			delete it->second;
		}

		delete rt;
		delete rtGauss;
	}

	void LightingSystem::loadShaders()
	{
		shaderLightTex = ShaderManager::addShader(
			"// FRAGMENT SHADER										\n\
			uniform sampler2D tex;									\n\
			uniform float lalpha;									\n\
			uniform vec4 ulcolor;									\n\
			float length(vec4 v)									\n\
			{														\n\
				return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);			\n\
			}														\n\
			void main()												\n\
			{														\n\
				vec4 src = texture2D(tex, gl_TexCoord[0].xy);		\n\
				src.a -= (length(src) - length(ulcolor)) * lalpha * 3.0;	\n\
				gl_FragColor = src;									\n\
			}",
			"// VERTEX SHADER										\n\
			void main()												\n\
			{														\n\
				gl_Position = ftransform();							\n\
				gl_TexCoord[0] = gl_MultiTexCoord0;					\n\
			}",
			"_ltMgr_");
		shaderLightTex->setUniform1i("texSrc", 0);
		shaderLightTex->setUniform1f("lalpha", 1.f);
		shaderLightTex->setUniform4f("ulcolor", 0.f, 0.f, 0.f, 1.f);

		shaderGauss = ShaderManager::addShader(
			"// FRAGMENT SHADER																		\n\
			// This shader is a modified version of Callum Hay's implementation:					\n\
			// http://callumhay.blogspot.no/2010/09/gaussian-blur-shader-glsl.html					\n\
			uniform float sigma;																	\n\
			uniform float blurSize; 																\n\
			uniform float lalpha;																	\n\
			uniform float pixels = 10.0;															\n\
			uniform vec4 ulcolor;																	\n\
			uniform vec2 direction;																	\n\
			uniform sampler2D blurSampler; 															\n\
																									\n\
			const float pi = 3.14159265;															\n\
																									\n\
			float length(vec4 v)																	\n\
			{																						\n\
				return sqrt(v.x*v.x + v.y*v.y + v.z*v.z );											\n\
			}																						\n\
																									\n\
			void main() {																			\n\
			  vec3 incrementalGaussian;																\n\
			  incrementalGaussian.x = 1.0 / (sqrt(2.0 * pi) * sigma);								\n\
			  incrementalGaussian.y = exp(-0.5 / (sigma * sigma));									\n\
			  incrementalGaussian.z = incrementalGaussian.y * incrementalGaussian.y;				\n\
			  vec4 avgValue = vec4(0.0);															\n\
			  float coefficientSum = 0.0;															\n\
			  avgValue += texture2D(blurSampler, gl_TexCoord[0].xy) * incrementalGaussian.x;		\n\
			  coefficientSum += incrementalGaussian.x;												\n\
			  incrementalGaussian.xy *= incrementalGaussian.yz;										\n\
			  for (float i = 1.0; i <= pixels; i++) { 												\n\
				avgValue += texture2D(blurSampler, gl_TexCoord[0].xy - i * blurSize *  				\n\
								direction) * incrementalGaussian.x;         						\n\
				avgValue += texture2D(blurSampler, gl_TexCoord[0].xy + i * blurSize *  				\n\
								direction) * incrementalGaussian.x;         						\n\
				coefficientSum += 2.0 * incrementalGaussian.x;										\n\
				incrementalGaussian.xy *= incrementalGaussian.yz;									\n\
			  }																						\n\
			  vec4 color = avgValue / coefficientSum;												\n\
			  color.a -= (length(color) - length(ulcolor)) * lalpha * 2.0;							\n\
			  gl_FragColor = color;																	\n\
			}",
			"// VERTEX SHADER																		\n\
			// This shader is a modified version of Callum Hay's implementation:					\n\
			// http://callumhay.blogspot.no/2010/09/gaussian-blur-shader-glsl.html					\n\
			void main() {																			\n\
				gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;								\n\
				gl_TexCoord[0] = gl_MultiTexCoord0;													\n\
			}",
			"_ltMgrGauss_"
			);
		shaderGauss->setUniform1i("blurSampler", 0);
		shaderGauss->setUniform1f("blurSize", 1.f/resolution.y);
		shaderGauss->setUniform1f("sigma", 5.f);
		shaderGauss->setUniform1f("lalpha", 1.f);
		shaderGauss->setUniform1f("pixels", 5.f);
		shaderGauss->setUniform4f("ulcolor", 0.f, 0.f, 0.f, 1.f);
	}

	void LightingSystem::setUnlitColor(Color c)
	{
		color = c;
		shaderLightTex->setUniform4f("ulcolor", c.r, c.g, c.b, c.a);
		shaderGauss->setUniform4f("ulcolor", c.r, c.g, c.b, c.a);
	}
	void LightingSystem::setLightAlpha(float a)
	{
		if (a > 1.f ) a = 1.f;
		if (a < 0.f) a = 0.f;

		a = 1.f - a;
		shaderLightTex->setUniform1f("lalpha", a);
		shaderGauss->setUniform1f("lalpha", a);
	}

	void LightingSystem::addLight(GameNode *node, LightDef *lDef)
	{
		if (lights.count(node))
			delete lights[node];

		lights[node] = lDef;

		if (lDef->lightType == 0)
			createFlatLightTexture(lDef);
		else if (lDef->lightType == 1)
			createSmoothLightTexture(lDef);
		else PimAssert(0, "Error: invalid light type!");
	}
	void LightingSystem::createSmoothLightTexture(LightDef *lightDef)
	{
		SmoothLightDef *lDef = (SmoothLightDef*)lightDef;

		// Generate the texture
		glGenTextures(1, &lDef->lTex);
		glBindTexture(GL_TEXTURE_2D, lDef->lTex);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, lDef->radius*2, lDef->radius*2, 
			0, GL_RGBA, GL_FLOAT, NULL);
		
		// Create a new framebuffer to avoid complications
		GLuint fbo;
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		// Attach the new texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
									GL_TEXTURE_2D, lDef->lTex, 0);
		
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glPushAttrib(GL_VIEWPORT_BIT);
		glViewport(0,0,lDef->radius*2, lDef->radius*2);
		glOrtho(0, lDef->radius*2, 0, lDef->radius*2, 0, 1);

		glDisable(GL_TEXTURE_2D);

		// Render the light texture
		Color ic = lDef->innerColor;
		Color oc = lDef->outerColor;

		glTranslatef((float)lDef->radius, (float)lDef->radius, 0.f);

		// The outer circle
		const float step = 6.283f/100.f;
		for (int i=0; i<lDef->innerPasses; i++)
		{
			glBegin(GL_TRIANGLE_FAN);
				glColor4d(ic.r, ic.g, ic.b, ic.a);
				glVertex2f(0.f, 0.f);
				glColor4f(oc.r, oc.g, oc.b, oc.a);
				for (float a=0.f; a+step<6.283f; a+=step)
				{
					glVertex2f(	cosf(a)*lDef->radius, 
								sinf(a)*lDef->radius);
				}
				glVertex2f(	cosf(0.f)*lDef->radius, 
							sinf(0.f)*lDef->radius);
			glEnd();
		}

		glEnable(GL_TEXTURE_2D);
		
		// Unbind
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteFramebuffers(1, &fbo);

		glPopAttrib();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void LightingSystem::createFlatLightTexture(LightDef *lightDef)
	{
		FlatLightDef *lDef = (FlatLightDef*)lightDef;

		if (lDef->falloff < 0.f)
			lDef->falloff = 0.f;

		/* REMOVED IN VERSION 0.4d */
		//else if (lDef->falloff > 1.f)
		//	lDef->falloff = 1.f;

		float totalRadius = lDef->radius + (lDef->radius * lDef->falloff);

		// Generate the texture
		glGenTextures(1, &lDef->lTex);
		glBindTexture(GL_TEXTURE_2D, lDef->lTex);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)totalRadius*2, (GLsizei)totalRadius*2, 
			0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		// Create a new framebuffer to avoid complications
		GLuint fbo = 0;
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		// Attach the new texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
									GL_TEXTURE_2D, lDef->lTex, 0);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glPushAttrib(GL_VIEWPORT_BIT);
		glViewport(0,0, (GLsizei)totalRadius*2, (GLsizei)totalRadius*2);
		glOrtho(0, totalRadius*2, 0, totalRadius*2, 0, 1);

		glDisable(GL_TEXTURE_2D);

		// Render the light texture
		Color ic = lDef->innerColor;
		Color oc = lDef->outerColor;

		glTranslatef(totalRadius, totalRadius, 0.f);


		// The inner circle
		const float step = 6.283f/100.f;
		glBegin(GL_TRIANGLE_FAN);
			glColor4f(ic.r, ic.g, ic.b, ic.a);
			glVertex2f(0.f, 0.f);
			for (float a=0.f; a+step<6.283f; a+=step)
			{
				glVertex2f(cosf(a)*lDef->radius, sinf(a)*lDef->radius);
			}
			glVertex2f(cosf(0.f)*lDef->radius, sinf(0.f)*lDef->radius);
		glEnd();

		if (lDef->falloff != 0.f)
		{
			// The falloff 
			glBegin(GL_TRIANGLE_STRIP);
				for (float a=0.f; a+step<6.283f; a+=step)
				{
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
		
		// Unbind
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteFramebuffers(1, &fbo);

		glPopAttrib();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		// Update the radius value
		lDef->radius = (int)totalRadius;
	}

	void LightingSystem::renderLightTexture()
	{
		Vec2 wd = GameControl::getSingleton()->getRenderWindow()->ortho;
		
		rt->bindFBO();
		rt->clear(GL_STENCIL_BUFFER_BIT);    

		renderLights();

		glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);

		glDisable(GL_TEXTURE_2D);
		glColor4f(color.r, color.g, color.b, color.a);
		glBegin(GL_QUADS);
			glVertex2f(0.f, 0.f);
			glVertex2f(resolution.x, 0.f);
			glVertex2f(resolution.x, resolution.y);
			glVertex2f(0.f, resolution.y);
		glEnd();
		glEnable(GL_TEXTURE_2D);  

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4ub(255,255,255,255);

		rt->unbindFBO();

		if (hqShadow)
		{
			gaussPass();
			rtGauss->bindTex();
		}
		else
		{	
			glUseProgram(shaderLightTex->getProgram());
			rt->bindTex();
		}

		glPushMatrix();				// Render to main FBO
		glLoadIdentity();

		glBegin(GL_QUADS);
			glTexCoord2i(0,0);		glVertex2f(0.f, 0.f);
			glTexCoord2i(1,0);		glVertex2f(wd.x, 0.f);
			glTexCoord2i(1,1);		glVertex2f(wd.x, wd.y);
			glTexCoord2i(0,1);		glVertex2f(0.f, wd.y);
		glEnd();

		glUseProgram(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();				// Render to main FBO
	}
	void LightingSystem::gaussPass()
	{
		shaderGauss->setUniform2f("direction", 0.f, 1.f);
		glUseProgram(shaderGauss->getProgram());

		rtGauss->bindFBO();
		rtGauss->clear();

		rt->bindTex();

		glBegin(GL_QUADS);
			glTexCoord2i(0,0);		glVertex2f(0.f, 0.f);
			glTexCoord2i(1,0);		glVertex2f(resolution.x, 0.f);
			glTexCoord2i(1,1);		glVertex2f(resolution.x, resolution.y);
			glTexCoord2i(0,1);		glVertex2f(0.f, resolution.y);
		glEnd();

		rtGauss->unbindFBO();
		
		glUseProgram(0);
		shaderGauss->setUniform2f("direction", 1.f, 0.f); 

		glUseProgram(shaderGauss->getProgram());
	}

	void LightingSystem::renderLights()
	{
		Vec2 renres = GameControl::getSingleton()->getCreationData().renderResolution;
		Vec2 coord = GameControl::getSingleton()->getCreationData().coordinateSystem;
		Vec2 posScale		= resolution / coord;		// The position (coord) scale
		Vec2 lineScale		= coord / renres;			// The shadow line initial position scale
		Vec2 renderScale	= renres / resolution;		// The final scale

		glEnable(GL_STENCIL_TEST);    

		glPushMatrix();						// Layer position & scale
		glScalef(posScale.x, posScale.y, 1.f);
		glTranslatef(parent->position.x, parent->position.y, 0.f);
		
		for (auto it=lights.begin(); it!=lights.end(); it++)
		{
			int r = it->second->radius;
			Vec2 p = it->first->getLightPosition();

			if (castShadow && it->second->castShadows)
				renderShadows(it->second, it->first, p, lineScale);

			glBindTexture(GL_TEXTURE_2D, it->second->lTex);
			glPushMatrix();					// Light texture
			glTranslatef(p.x, p.y, 0.f);

			glStencilFunc(GL_NOTEQUAL, 0x1, 0x1);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

			glColor4f(1.f, 1.f, 1.f, 0.2f);

			//glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
			glBegin(GL_QUADS);
				glTexCoord2i(0,0);glVertex2i(-r,-r);
				glTexCoord2i(1,0);glVertex2i(r,-r);
				glTexCoord2i(1,1);glVertex2i(r,r);
				glTexCoord2i(0,1);glVertex2i(-r,r);
			glEnd();
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glPopMatrix();					// Light texture
		}

		glPopMatrix();						// Layer position & scale
			
		glDisable(GL_STENCIL_TEST);
	}
	void LightingSystem::renderShadows(LightDef *ld, GameNode *light, Vec2 &pos, Vec2 &sc)
	{
		// PARAMETERS:
		//	ld:			The light definition struct
		//	light:		The game node, acting as a light
		//	pos:		The position of the light relative to it's parent
		//	sc:			The scale (lightingSys.resolution / renderResolution)

		int r = ld->radius;
		std::vector<Line*> castLines;


		glPushMatrix();				// Shadows
		glTranslatef(pos.x, pos.y, 0.f);

		glClear(GL_STENCIL_BUFFER_BIT);

		glStencilFunc(GL_NEVER, 0x1, 0x0);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);	

		glDisable(GL_TEXTURE_2D);
		glColor4f(color.r, color.g, color.b, 1.f);

#ifdef _DEBUG					// IFDEF DEBUG
		if (dbgDrawNormal)		
		{
			glLineWidth(2.f);
			glColor4f(0.f, 1.f, 0.f, 1.f);
			glDisable(GL_STENCIL_TEST);
			glBegin(GL_LINES);
		}						
#endif							// ENDIF

		for (unsigned int i=0; i<casters.size(); i++)
		{
			auto lines = casters[i]->shadowShape->lines;
			{
				for (unsigned int i=0; i<lines.size(); i++)
				{
					if ((pos-lines[i]->getMid(sc)).length() <= r)
					{
						if (lines[i]->isFacing(pos, sc))
						{
							castLines.push_back(lines[i]);

#ifdef _DEBUG									// IFDEF DEBUG
							if (dbgDrawNormal)	
							{
								glVertex2f(lines[i]->getP1(sc).x-pos.x, lines[i]->getP1(sc).y-pos.y);
								glVertex2f(lines[i]->getP2(sc).x-pos.x, lines[i]->getP2(sc).y-pos.y);

								glVertex2f(lines[i]->getMid(sc).x-pos.x, lines[i]->getMid(sc).y-pos.y);
								glVertex2f(lines[i]->getNormalEnd(sc).x-pos.x, 
											lines[i]->getNormalEnd(sc).y-pos.y);
							}					
#endif											// ENDIF
						}
					}
				}
			}
		}

#ifdef _DEBUG					// IFDEF DEBUG
		if (dbgDrawNormal)		
		{
			glEnd();
			glEnable(GL_STENCIL_TEST);
			glColor4f(color.r, color.g, color.b, 1.f);
			glLineWidth(1.f);
		}						
#endif							// ENDIF

		for (unsigned int i=0; i<castLines.size(); i++)
		{
			Vec2  v1 = (pos-castLines[i]->getP1(sc)),
				  v2 = (pos-castLines[i]->getP2(sc));
			float a1 = v1.angleBetween(Vec2(1.f,0.f)),
				  a2 = v2.angleBetween(Vec2(1.f,0.f));

			glBegin(GL_QUADS);
				glVertex2f(-v1.x, -v1.y);
				glVertex2f(-v2.x, -v2.y);

				v1 += Vec2( cosf(a1*((float)M_PI/180.f)), 
							sinf(a1*((float)M_PI/180.f))) * (float)r * 100.f;
				v2 += Vec2( cosf(a2*((float)M_PI/180.f)), 
							sinf(a2*((float)M_PI/180.f))) * (float)r * 100.f;

				glVertex2f(-v2.x, -v2.y);
				glVertex2f(-v1.x, -v1.y);
			glEnd();
		}

		glColor4f(1.f, 1.f, 1.f, 1.f);
		glEnable(GL_TEXTURE_2D);

		glPopMatrix();				// Shadows
	}
}