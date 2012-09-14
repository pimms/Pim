#include "StdAfx.h"

#include "PimLayer.h"
#include "PimGameNode.h"
#include "PimVec2.h"
#include "PimGameControl.h"
#include "PimException.h"

namespace Pim
{
	Layer* Layer::topLayer = NULL;

	Layer::Layer(void)
	{
		isTopLayer	= false;
		color		= Color(1.f, 1.f, 1.f, 1.f);
		immovable	= false;
		scale		= Vec2(1.f, 1.f);
		lightSys	= NULL;
	}
	Layer::~Layer(void)
	{
		if (isTopLayer)
			topLayer = NULL;
		destroyLightingSystem();
	}

	Layer* Layer::getTopLayer()
	{
		return topLayer;
	}

	Vec2 Layer::getWorldPosition()
	{
		if (immovable)
			return position;
		
		if (parent)
			return position + parent->getWorldPosition();

		return position;
	}
	float Layer::getWorldRotation()
	{
		if (parent)
			return rotation + parent->getWorldRotation();

		return rotation;
	}

	void Layer::immovableLayer(bool immov)
	{
		immovable = immov; 
	}

	void Layer::draw()
	{
		glPushMatrix();

		if (immovable)
		{
			glLoadIdentity();
		}

		// Update position
		Vec2 fac = GameControl::getSingleton()->forcedCoordinateFactor();

		if (allowMidPixelPosition)
			glTranslatef(position.x / fac.x, position.y / fac.y, 0.f);
		else
			glTranslatef(floor(position.x) / fac.x, position.y / fac.y, 0.f);

		glRotatef(rotation, 0.f, 0.f, 1.f);

		orderChildren();

		for (unsigned int i=0; i<children.size(); i++)
		{
			children[i]->draw();
		}

		glTranslatef(200.f, 200.f, 0.f);

		/*
		glDisable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
			glColor3f(0.f,1.f,0.f);			// Green
			glVertex2f(-50.f, -50.f);
			glVertex2f(50.f, -50.f);
			glVertex2f(50.f, 50.f);

			glColor3f(1.f, 0.f, 0.f);		// Red
			glVertex2f(-50.f, 50.f);
		glEnd();
		glEnable(GL_TEXTURE_2D);
		*/

		if (lightSys)
			lightSys->renderLightTexture();

		glPopMatrix();
	}

	void Layer::createLightingSystem()
	{
		destroyLightingSystem();
		lightSys = new LightingSystem(this);
	}
	void Layer::destroyLightingSystem()
	{
		if (lightSys)
			delete lightSys;
		lightSys = NULL;
	}
	void Layer::addLight(GameNode *node, LightDef *lDef)
	{
		if (lightSys)
			lightSys->addLight(node, lDef);
	}
	void Layer::removeLight(GameNode *node)
	{
		if (lightSys->lights.count(node))
		{
			delete lightSys->lights[node];
			lightSys->lights.erase(node);
		}
	}
	void Layer::addShadowCaster(Sprite *caster)
	{
		lightSys->casters.push_back(caster);
	}
	void Layer::removeShadowCaster(Sprite *caster)
	{
		for (unsigned int i=0; i<lightSys->casters.size(); i++)
		{
			if (lightSys->casters[i] == caster)
				lightSys->casters.erase(lightSys->casters.begin() + i);
		}
	}
	void Layer::setCastShadows(bool shadows)
	{
		if (lightSys)
			lightSys->castShadow = true;
	}
	void Layer::setShadowTechnique(ShadowTechnique::ShadowTechnique tech)
	{
		if (lightSys)
			lightSys->tech = tech;
	}
	void Layer::setLightMultiplicationShaderActive(bool flag)
	{
		if (lightSys)
			lightSys->useMultShader = flag;
	}
	LightingSystem* Layer::getLightingSystem()
	{
		return lightSys;
	}

	void Layer::_topLevelNode()
	{
		PimAssert(!topLayer, "Error: another top layer already exist!");
		isTopLayer	= true;
		topLayer	= this;
	}
}