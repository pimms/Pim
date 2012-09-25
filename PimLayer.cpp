#include "StdAfx.h"

#include "PimLayer.h"
#include "PimGameNode.h"
#include "PimVec2.h"
#include "PimGameControl.h"
#include "PimException.h"
#include "PimCollisionManager.h"

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

	Layer* Layer::getParentLayer()
	{
		return this;
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

	Vec2 Layer::getLayerPosition()
	{
		return Vec2(0.f, 0.f);
	}

	void Layer::setImmovableLayer(bool immov)
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

		if (lightSys)
			lightSys->renderLightTexture();

		glPopMatrix();
	}

	void Layer::createLightingSystem(Vec2 resolution)
	{
		destroyLightingSystem();
		lightSys = new LightingSystem(this, resolution);
	}
	void Layer::destroyLightingSystem()
	{
		if (!lightSys)
			return;

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
			removeChild(node, true);
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
			lightSys->castShadow = shadows;
	}
	void Layer::setLightingUnlitColor(Color color)
	{
		if (lightSys)
			lightSys->color = color;
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
	
	void Layer::addCollisionNode(GameNode *node)
	{
		PimAssert(node->colShape, "Error: collision node has no collision shape!");
		collisionNodes.push_back(node);
	}
	void Layer::removeCollisionNode(GameNode *node)
	{
		for (unsigned int i=0; i<collisionNodes.size(); i++)
		{
			if (collisionNodes[i] == node)
				collisionNodes.erase(collisionNodes.begin() + i);
		}
	}

	void Layer::_topLevelNode()
	{
		PimAssert(!topLayer, "Error: another top layer already exist!");
		isTopLayer	= true;
		topLayer	= this;
	}
}