#include "Stdafx.h"

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
		useMultShader	= true;
		resolution		= pResolution;

		// Create the texture
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, resolution.x, resolution.y, 
						0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		
		// Create the main framebuffer
		glGenFramebuffersEXT(1, &frameBuffer);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBuffer);

		// Attach the texture
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, 
										GL_TEXTURE_2D, texID, 0);

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		loadShaders();
	}
	LightingSystem::~LightingSystem()
	{
		for (auto it=lights.begin(); it!=lights.end(); it++)
		{
			delete it->second;
		}

		glDeleteTextures(1, &texID);
		glDeleteFramebuffersEXT(1, &frameBuffer);
	}

	void LightingSystem::loadShaders()
	{
		ShaderManager::addShader(
			"uniform sampler2D texSrc;				\n\
			uniform sampler2D texDst;				\n\
			void main()								\n\
			{										\n\
				vec4 src = texture2D(texSrc, gl_TexCoord[0]);	\n\
				vec4 dst = texture2D(texDst, gl_TexCoord[1]);	\n\
				gl_FragColor = src + src;		\n\
			}",
			"void main()							\n\
			{										\n\
				gl_Position = ftransform();			\n\
				gl_TexCoord[0] = gl_MultiTexCoord0;	\n\
				gl_TexCoord[1] = gl_MultiTexCoord0; \n\
			}",
			"_ltMgr_", Quality::MED);

		shader = ShaderManager::getShader("_ltMgr_", Quality::MED);

		shader->setUniform1i("texSrc", 0);
		shader->setUniform1i("texDst", 1);
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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, lDef->radius*2, lDef->radius*2, 
			0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		// Create a new framebuffer to avoid complications
		GLuint fbo;
		glGenFramebuffersEXT(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER_EXT, fbo);

		// Attach the new texture
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
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
		glBegin(GL_TRIANGLE_FAN);
			glColor4f(oc.r, oc.g, oc.b, oc.a);
			glVertex2f(0.f, 0.f);
			//glColor4f(oc.r, oc.g, oc.b, oc.a);
			for (float a=0.f; a+step<6.283f; a+=step)
			{
				glVertex2f(cosf(a)*lDef->radius, sinf(a)*lDef->radius);
			}
			glVertex2f(cosf(0.f)*lDef->radius, sinf(0.f)*lDef->radius);
		glEnd();

		for (int i=0; i<lDef->innerPasses; i++)
		{
			float fac = 1.f;// (i+1)/lDef->innerPasses;
			glBegin(GL_TRIANGLE_FAN);
				glColor4f(ic.r, ic.g, ic.b, ic.a);
				glVertex2f(0.f, 0.f);
				glColor4f(ic.r, ic.g, ic.b, 0.f);
				for (float a=0.f; a+step<6.283f; a+=step)
				{
					glVertex2f(	cosf(a)*lDef->radius*lDef->falloff * fac, 
								sinf(a)*lDef->radius*lDef->falloff * fac);
				}
				glVertex2f(	cosf(0.f)*lDef->radius*lDef->falloff * fac, 
							sinf(0.f)*lDef->radius*lDef->falloff * fac);
			glEnd();
		}                         

		glEnable(GL_TEXTURE_2D);
		
		// Unbind
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteFramebuffersEXT(1, &fbo);

		glPopAttrib();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}
	void LightingSystem::createFlatLightTexture(LightDef *lightDef)
	{
		FlatLightDef *lDef = (FlatLightDef*)lightDef;

		if (lDef->falloff < 0.f)
			lDef->falloff = 0.f;
		else if (lDef->falloff > 1.f)
			lDef->falloff = 1.f;

		float totalRadius = lDef->radius + (lDef->radius * lDef->falloff);

		// Generate the texture
		glGenTextures(1, &lDef->lTex);
		glBindTexture(GL_TEXTURE_2D, lDef->lTex);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, totalRadius*2, totalRadius*2, 
			0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		// Create a new framebuffer to avoid complications
		GLuint fbo;
		glGenFramebuffersEXT(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER_EXT, fbo);

		// Attach the new texture
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
									GL_TEXTURE_2D, lDef->lTex, 0);
		
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glPushAttrib(GL_VIEWPORT_BIT);
		glViewport(0,0,totalRadius*2, totalRadius*2);
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
					// Use the inner color alpha 
					glColor4f(oc.r, oc.g, oc.b, ic.a);
					glVertex2f(cosf(a)*lDef->radius, sinf(a)*lDef->radius);

					// Then zero alpha to fade out.
					glColor4f(oc.r, oc.g, oc.b, 0.f);
					glVertex2f(cosf(a)*totalRadius, sinf(a)*totalRadius);
				}

			glColor4f(oc.r, oc.g, oc.b, ic.a);
			glVertex2f(cosf(0.f)*lDef->radius, sinf(0.f)*lDef->radius);

			glColor4f(oc.r, oc.g, oc.b, 0.f);
			glVertex2f(cosf(0.f)*totalRadius, sinf(0.f)*totalRadius);

			glEnd();
		}

		glEnable(GL_TEXTURE_2D);
		
		// Unbind
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteFramebuffersEXT(1, &fbo);

		glPopAttrib();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}

	void LightingSystem::renderLightTexture()
	{
		Vec2 wd = GameControl::getSingleton()->getRenderWindow()->ortho;
		Vec2 off = GameControl::getSingleton()->getRenderWindow()->orthoOff;
				
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBuffer);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		// Set the ortho and viewport to match the texture's size
		glPushAttrib(GL_VIEWPORT_BIT);
		glViewport(0,0,resolution.x, resolution.y);
		glOrtho(0, resolution.x, 0, resolution.y, 0, 1);

		glTranslatef(0.f, 0.f, 0.f);

		glClear(GL_COLOR_BUFFER_BIT);
		
		glDisable(GL_TEXTURE_2D);
		glColor4f(color.r, color.g, color.b, color.a);
		glBegin(GL_QUADS);
			glVertex2f(0.f, 0.f);
			glVertex2f(resolution.x, 0.f);
			glVertex2f(resolution.x, resolution.y);
			glVertex2f(0.f, resolution.y);
		glEnd();
		glEnable(GL_TEXTURE_2D);                     

		glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
		_renderLights();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glBindTexture(GL_TEXTURE_2D, texID);

		// Restore the matrices
		glPopAttrib();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glColor4ub(255,255,255,255);
		glBegin(GL_QUADS);
			glTexCoord2i(0,0);
			glVertex2f(0.f, 0.f);

			glTexCoord2i(1,0);
			glVertex2f(wd.x, 0.f);

			glTexCoord2i(1,1);
			glVertex2f(wd.x, wd.y);

			glTexCoord2i(0,1);
			glVertex2f(0.f, wd.y);
		glEnd();

		glPopMatrix();

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	
	void LightingSystem::_renderLights()
	{
		glColor4ub(255,255,255,255);

		Vec2 sc = resolution / GameControl::getSingleton()->getCreationData().coordinateSystem;
		
		//if (useMultShader)
		//{
		//	glActiveTexture(GL_TEXTURE1);
		//	glBindTexture(GL_TEXTURE_2D, texID);
		//	glUseProgram(shader->getProgram());
		//	glActiveTexture(GL_TEXTURE0);
		//}
		

		for (auto it=lights.begin(); it!=lights.end(); it++)
		{
			float r = it->second->radius;
			Vec2 p = it->first->getLightPosition() + parent->position;
			p *= sc;

			glPushMatrix();
			//glLoadIdentity();
			glTranslatef(floor(p.x), floor(p.y), 0.f);

			glBindTexture(GL_TEXTURE_2D, it->second->lTex);

			glBegin(GL_QUADS);
				glTexCoord2i(0,0);
				glVertex2f(-r,-r);

				glTexCoord2i(1,0);
				glVertex2f(r,-r);
				
				glTexCoord2i(1,1);
				glVertex2f(r,r);

				glTexCoord2i(0,1);
				glVertex2f(-r,r);
			glEnd();

			glPopMatrix();
		}
	}
	

	/* // OLD IMPLEMENTATION
	void LightingSystem::_renderLights()
	{
		for (auto it=lights.begin(); it!=lights.end(); it++)
		{
			glPushMatrix();

			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBuffer);

			Vec2 lpos = it->first->getLayerPosition();
			glTranslatef(	lpos.x+parent->position.x, 
							lpos.y+parent->position.y, 0.f);
			
			int r = it->second->radius;

			glPointSize(5.f);
			glColor4ub(0,255,150,255);
			glDisable(GL_TEXTURE_2D);


			glLineWidth(4.f);
			glBegin(GL_LINES);

			// The vector containg lines that should cast shadow
			std::vector<Line*> castLines;

			for (unsigned int i=0; i<casters.size(); i++)
			{
				auto lines = casters[i]->shadowShape->lines;
				{
					for (unsigned int i=0; i<lines.size(); i++)
					{
						if ((lpos-lines[i]->getMid()).length() <= r)
						{
							if (lines[i]->isFacing(lpos))
							{
								castLines.push_back(lines[i]);

								glVertex2f(lines[i]->getP1().x-lpos.x, lines[i]->getP1().y-lpos.y);
								glVertex2f(lines[i]->getP2().x-lpos.x, lines[i]->getP2().y-lpos.y);

								glVertex2f(lines[i]->getMid().x-lpos.x, lines[i]->getMid().y-lpos.y);
								glVertex2f(lines[i]->getNormalEnd().x-lpos.x, 
										   lines[i]->getNormalEnd().y-lpos.y);
							}
						}
					}
				}
			}

			glEnd();
			

			
			// Now we need to render the light.
			
			glColor4ub(255,255,255,255);
			glEnable(GL_TEXTURE_2D);

			if (useMultShader)
			{
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, texID);

				glUseProgram(shader->getProgram());
			}

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, it->second->lTex);

			glBegin(GL_QUADS);
				glVertex2f(-r, -r);
				glTexCoord2i(0,0);

				glVertex2f(r, -r);
				glTexCoord2i(1,0);

				glVertex2f(r, r);
				glTexCoord2i(1,1);

				glVertex2f(-r, r);
				glTexCoord2i(0,1);
			glEnd();

			glUseProgram(0);

			// The following renders the shadows casted by this light.

			// THIS NEEDS TO BE RENDERED ONTO A SEPARATE FRAMEBUFFER, AND THEN
			// RENDERED TO THE MAIN FRAMEBUFFER AS THE LAST STEP OF THE 
			// ALGORITHM. SOME SWEET BLEND FUNC MUST BE UTILIZED. ALSO
			// SOME SWEET SHADER HAS TO BE USED IN ORDER TO MAKE THE DARKNESS
			// AND LIGHT COLORS CUSTOMIZEABLE.

			continue;

			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, shadowFBO);

			glDisable(GL_TEXTURE_2D);
			glColor4ub(0,0,0,255);
			for (unsigned int i=0; i<castLines.size(); i++)
			{
				Vec2 v1 = (lpos-castLines[i]->getP1());
				Vec2 v2 = (lpos-castLines[i]->getP2());
				float a1 = v1.angleBetween(Vec2(1.f,0.f));
				float a2 = v2.angleBetween(Vec2(1.f,0.f));

				glBegin(GL_QUADS);
					glVertex2f(-v1.x, -v1.y);
					glVertex2f(-v2.x, -v2.y);

					v1 += Vec2(cosf(a1*(M_PI/180.f)), sinf(a1*(M_PI/180.f))) * r;
					v2 += Vec2(cosf(a2*(M_PI/180.f)), sinf(a2*(M_PI/180.f))) * r;


					
					glVertex2f(-v2.x, -v2.y);
					glVertex2f(-v1.x, -v1.y);
				glEnd();
			}
			glEnable(GL_TEXTURE_2D);

			glPopMatrix();
		}

		return;
		// Render the shadow FBO
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, shadowFBO);
		glBindTexture(GL_TEXTURE_2D, 0);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glPushAttrib(GL_VIEWPORT_BIT);
		glViewport(0,0,800,600);

		glOrtho(0, 800, 0, 600, 0, 1);

		glDisable(GL_TEXTURE_2D);
		glColor4ub(255,0,0,255);
		glBegin(GL_QUADS);
			glVertex2f(0.f, 0.f);
			glVertex2f(800.f, 0.f);
			glVertex2f(800.f, 600.f);
			glVertex2f(0.f, 600.f);
		glEnd();
		glEnable(GL_TEXTURE_2D);

		glPopAttrib();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		
		glBindTexture(GL_TEXTURE_2D, shadowTex);

		glBegin(GL_QUADS);
			glVertex2f(0.f, 0.f);
			glTexCoord2i(0,0);
			glVertex2f(800.f, 0.f);
			glTexCoord2i(1,0);
			glVertex2f(800.f,600.f);
			glTexCoord2i(1,1);
			glVertex2f(0.f, 600.f);
			glTexCoord2i(0,1);
		glEnd();
	}
	*/
}