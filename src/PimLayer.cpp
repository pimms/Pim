#include "PimInternal.h"

#include "PimLightDef.h"
#include "PimLayer.h"
#include "PimGameNode.h"
#include "PimVec2.h"
#include "PimGameControl.h"
#include "PimAssert.h"
#include "PimCollisionManager.h"
#include "PimScene.h"

namespace Pim
{
	Layer::Layer(void)
	{
		color		= Color(1.f, 1.f, 1.f, 1.f);
		immovable	= false;
		scale		= Vec2(1.f, 1.f);
		lightSys	= NULL;
		parentScene = NULL;
	}
	Layer::~Layer(void)
	{
		destroyLightingSystem();
	}

	Scene* Layer::getParentScene()
	{
		if (parent)
		{
			return parent->getParentScene();
		}
		else 
		{
			return parentScene;
		}
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
		Vec2 fac = GameControl::getSingleton()->coordinateFactor();

		if (allowMidPixelPosition)
		{
			glTranslatef(position.x / fac.x, position.y / fac.y, 0.f);
		}
		else
		{
			glTranslatef(floor(position.x) / fac.x, position.y / fac.y, 0.f);
		}

		glRotatef(rotation, 0.f, 0.f, 1.f);

		fac = GameControl::getSingleton()->windowScale();
		glScalef(scale.x, scale.y, 1.f);

		orderChildren();

		for (unsigned int i=0; i<children.size(); i++)
		{
			children[i]->draw();
		}

		if (lightSys)
		{
			lightSys->renderLightTexture();
		}

		glPopMatrix();
	}

	void Layer::setZOrder(int z)
	{
		if (parent)
			parent->dirtyZOrder = true;
		else if (parentScene)
			parentScene->dirtyZOrder = true;
		
		zOrder = z;
	}


	// ---------- LIGHTING SYSTEM METHODS ----------


	void Layer::createLightingSystem(Vec2 resolution)
	{
		destroyLightingSystem();
		lightSys = new LightingSystem(this, resolution);
	}
	void Layer::destroyLightingSystem()
	{
		if (lightSys)
		{
			delete lightSys;
			lightSys = NULL;
		}
	}
	void Layer::addLight(GameNode *node, LightDef *lDef)
	{
		PimAssert(node->parent != NULL, "ERROR: Orphan lights are not allowed");

		if (lightSys)
			lightSys->addLight(node, lDef);
	}
	bool Layer::addLight(GameNode *node, PreloadLightDef *pld, std::string identifier)
	{
		if (lightSys)
		{
			if (!lightSys->usePreloadedTexture(pld, identifier))
			{
				return false;
			}

			addLight(node, pld);
		}

		return true;
	}
	void Layer::removeLight(GameNode *node)
	{
		if (lightSys && lightSys->lights.count(node))
		{
			delete lightSys->lights[node];
			lightSys->lights.erase(node);
		}
	}
	void Layer::preloadLightTexture(LightDef *ld, std::string identifier)
	{
		if (ld)
		{
			if (lightSys)
			{
				lightSys->preloadTexture(ld, identifier);
			}

			delete ld;
		}
	}
	void Layer::addShadowCaster(GameNode *caster)
	{
		if (lightSys)
			lightSys->casters.push_back(caster);
	}
	void Layer::removeShadowCaster(GameNode *caster)
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
			lightSys->setUnlitColor(color);
	}
	void Layer::setLightAlpha(float a)
	{
		if (lightSys)
			lightSys->setLightAlpha(a);
	}
	void Layer::setSmoothShadows(bool flag)
	{
		if (lightSys)
			lightSys->hqShadow = flag;
	}
	void Layer::setShadowcasterDebugDraw(bool flag)
	{
		if (lightSys)
			lightSys->dbgDrawNormal = flag;
	}
	LightingSystem* Layer::getLightingSystem()
	{
		return lightSys;
	}
	

	// ---------------------------------------------


	/* // THE INTEGRATED COLLISION LIBRARY IS DEPRECATED
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
	*/
}