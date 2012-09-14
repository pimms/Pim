#include "Stdafx.h"

#include "PimVec2.h"
#include "PimLightingSystem.h"
#include "PimRenderWindow.h"
#include "PimShaderManager.h"
#include "PimGameControl.h"
#include "PimGameNode.h"
#include "PimLayer.h"


namespace Pim
{
	LightDef::LightDef()
	{
		innerColor		= Color(1.f, 1.f, 1.f, 1.f);
		outerColor		= Color(0.2f, 0.2f, 0.2f, 0.0f);
		castShadow		= true;
		radius			= 128;
		lTex			= NULL;
		innerFalloff	= 0.2f;
	}


	LightingSystem::LightingSystem(Layer *par)
	{
		parent			= par;
		castShadow		= true;
		useMultShader	= true;

		// Create the texture
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		// Create the framebuffer
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
			delete it->first;
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

		createLightTexture(lDef);
	}
	void LightingSystem::createLightTexture(LightDef *lDef)
	{
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
		
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_TEXTURE_2D);

		// Render the light texture
		Color ic = lDef->innerColor;
		Color oc = lDef->outerColor;

		glTranslatef((float)lDef->radius, (float)lDef->radius, 0.f);

		// The outer circle
		glBegin(GL_TRIANGLE_FAN);
		glColor4f(oc.r, oc.g, oc.b, ic.a*0.7f);
		glVertex2f(0.f, 0.f);
		glColor4f(oc.r, oc.g, oc.b, 0.f);
		const float step = 6.283f/100.f;
		for (float a=0.f; a+step<6.283f; a+=step)
		{
			glVertex2f(cosf(a)*lDef->radius, sinf(a)*lDef->radius);
		}
		glVertex2f(cosf(0.f)*lDef->radius, sinf(0.f)*lDef->radius);
		glEnd();

		float fac = 1.f;
		for (int i=0; i<3; i++)
		{
			glBegin(GL_TRIANGLE_FAN);
			glColor4f(ic.r, ic.g, ic.b, ic.a);
			glVertex2f(0.f, 0.f);
			glColor4f(ic.r, ic.g, ic.b, 0.f);
			for (float a=0.f; a+step<6.283f; a+=step)
			{
				glVertex2f(	cosf(a)*lDef->radius*lDef->innerFalloff * fac, 
							sinf(a)*lDef->radius*lDef->innerFalloff * fac);
			}
			glVertex2f(	cosf(0.f)*lDef->radius*lDef->innerFalloff * fac, 
						sinf(0.f)*lDef->radius*lDef->innerFalloff * fac);
			glEnd();

			fac -= 0.25f;
		}

		glEnable(GL_TEXTURE_2D);
		
		// Unbind
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteFramebuffersEXT(1, &fbo);

		glPopMatrix();
	}

	void LightingSystem::renderLightTexture()
	{
		Vec2 wd = GameControl::getSingleton()->getWindowSize();
		
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBuffer);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0,wd.x,0,wd.y,-1,1);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glPushAttrib(GL_VIEWPORT_BIT);
		glViewport(0.f,0.f,(GLsizei)wd.x,(GLsizei)wd.y);

		glClear(GL_COLOR_BUFFER_BIT);

		glDisable(GL_TEXTURE_2D);

		glColor4ub(0,0,0,255);		// Render the darkness
		glBegin(GL_QUADS);					
			glVertex2f(0.f, 0.f);
			glVertex2f(wd.x, 0.f);
			glVertex2f(wd.x, wd.y);
			glVertex2f(0.f,  wd.y);
		glEnd();
		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glColor4ub(255,255,255,255);		// Render the lights
		glEnable(GL_TEXTURE_2D);
		_renderLights();					
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		glPopAttrib();
														
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		
		glLoadIdentity();
		glBindTexture(GL_TEXTURE_2D, texID);

		glColor4ub(255,255,255,255);		// Render the framebuffer
		glBegin(GL_QUADS);
			glTexCoord2i(0,0); glVertex2f(0.f,  0.f);
			glTexCoord2i(1,0); glVertex2f(wd.x, 0.f);
			glTexCoord2i(1,1); glVertex2f(wd.x, wd.y);
			glTexCoord2i(0,1); glVertex2f(0.f,  wd.y);
		glEnd();

		glPopMatrix();
	}
	void LightingSystem::_renderLights()
	{
		for (auto it=lights.begin(); it!=lights.end(); it++)
		{
			glPushMatrix();

			Vec2 lpos = it->first->position + parent->position;
			glTranslatef(lpos.x, lpos.y, 0.f);
			
			int r = it->second->radius;

			glPointSize(5.f);
			glColor4ub(255,0,0,255);
			glDisable(GL_TEXTURE_2D);

			for (unsigned int i=0; i<casters.size(); i++)
			{
				
			}
			
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

			glPopMatrix();
		}
	}
}