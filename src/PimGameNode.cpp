#include "PimInternal.h"

#include "PimVec2.h"
#include "PimGameNode.h"
#include "PimException.h"
#include "PimInput.h"
#include "PimGameControl.h"
#include "PimLayer.h"
#include "PimPolygonShape.h"
#include "PimCollisionManager.h"
#include "PimScene.h"
#include "PimLightingSystem.h"

#include <iostream>

namespace Pim
{

	GameNode::GameNode()
	{
		parent					= NULL;
		rotation				= 0.f;
		allowMidPixelPosition	= true;
		zOrder					= 0;
		dirtyZOrder				= false;

		shadowShape		= NULL;
		dbgShadowShape	= false;

		//colShape				= NULL;
		//dbgColShape				= false;

		//colGroup				= 1|2|4|8;
		//colFilter				= 1|2|4|8;

		userData				= NULL;
	}
	GameNode::~GameNode()
	{
		if (getParentLayer())
		{
			getParentLayer()->removeLight(this);
		}

		if (shadowShape)
		{
			delete shadowShape;
		}

		removeAllChildren(true);
		
		unlistenFrame();
		unlistenInput();
		unlistenController();
	}

	void GameNode::addChild(GameNode *ch)
	{
#ifdef _DEBUG
		PimAssert(!ch->getParent(), "Node already has a parent");
#endif

		ch->parent = this;
		children.push_back(ch);

		dirtyZOrder = true;
	}
	void GameNode::removeChild(GameNode *ch, bool cleanup)
	{
		for (unsigned int i=0; i<children.size(); i++)
		{
			if (children[i] == ch)
			{
				children.erase(children.begin() + i);

				if (cleanup) 
				{
					delete ch;
				}
				else
				{
					ch->parent = NULL;
				}

				return;
			}
		}
	}
	void GameNode::removeAllChildren(bool cleanup)
	{
		// Delete all if required
		for each (GameNode *child in children)
		{
			if (cleanup) 
				delete child;
			else
				child->parent = NULL;
		}

		// THEN clear the array
		children.clear();
	}

	GameNode* GameNode::getParent()
	{
		return parent;
	}
	Scene* GameNode::getParentScene()
	{
		if (parent)
		{
			return parent->getParentScene();
		}
		return NULL;
	}
	Layer* GameNode::getParentLayer()
	{
		if (parent)
		{
			return parent->getParentLayer();
		}
		return NULL;
	}
	
	void GameNode::listenInput()
	{
		GameControl::getSingleton()->addKeyListener(this);
		GameControl::getSingleton()->addMouseListener(this);
	}
	void GameNode::unlistenInput()
	{
		GameControl::getSingleton()->removeKeyListener(this);
		GameControl::getSingleton()->removeMouseListener(this);
	}

	void GameNode::listenKeys()
	{
		GameControl::getSingleton()->addKeyListener(this);
	}
	void GameNode::unlistenKeys()
	{
		GameControl::getSingleton()->removeKeyListener(this);
	}

	void GameNode::listenMouse()
	{
		GameControl::getSingleton()->addMouseListener(this);
	}
	void GameNode::unlistenMouse()
	{
		GameControl::getSingleton()->removeMouseListener(this);
	}

	void GameNode::listenController()
	{
		GameControl::getSingleton()->addControlListener(this);
	}
	void GameNode::unlistenController()
	{
		GameControl::getSingleton()->removeControlListener(this);
	}

	void GameNode::listenFrame()
	{
		GameControl::getSingleton()->addFrameListener(this);
	}
	void GameNode::unlistenFrame()
	{
		GameControl::getSingleton()->removeFrameListener(this);
	}

	Vec2 GameNode::getWorldPosition()
	{
		return position + parent->getWorldPosition();
	}
	float GameNode::getWorldRotation()
	{
		return rotation + parent->getWorldRotation();
	}

	Vec2 GameNode::getLayerPosition()
	{
		return position + parent->getLayerPosition();
	}
	Vec2 GameNode::getLightPosition()
	{
		return getLayerPosition() + lightPosition;
	}

	void GameNode::orderChildren()
	{
		if (!dirtyZOrder || children.size() <= 1)
			return;
		
		// Insertion sorting - the children should be somewhat sorted already.
		for (unsigned int j=1; j<children.size(); j++)
		{
			GameNode *key = children[j];
			int i = j - 1;

			while (i >= 0 && children[i]->zOrder > key->zOrder)
			{
				children[i+1] = children[i];
				i--;
			}
			children[i+1] = key;
		}

		dirtyZOrder = false;
	}

	void GameNode::draw()
	{
		glPushMatrix();
		Vec2 fac = GameControl::getSingleton()->coordinateFactor();

		if (allowMidPixelPosition)
		{
			glTranslatef(position.x / fac.x, position.y / fac.y, 0.f);
		}
		else
		{
			glTranslatef(floor(position.x) / fac.x, floor(position.y) / fac.y, 0.f);
		}

		glRotatef(rotation, 0.f, 0.f, 1.f);

		// Debug draw shadow shape if flagged to do so
		if (shadowShape && dbgShadowShape)
		{
			shadowShape->debugDraw();
		}

		orderChildren();
		for (unsigned int i=0; i<children.size(); i++)
		{
			children[i]->draw();
		}

		glPopMatrix();
	}
	void GameNode::batchDraw()
	{
		glPushMatrix();
		Vec2 fac = GameControl::getSingleton()->coordinateFactor();

		// Translate
		if (allowMidPixelPosition)
		{
			glTranslatef(position.x / fac.x, position.y / fac.y, 0.f);
		}
		else
		{
			glTranslatef(floor(position.x) / fac.x, floor(position.y) / fac.y, 0.f);
		}

		// Rotate
		glRotatef(rotation, 0.f, 0.f, 1.f);

		// Debug draw shadow shape if flagged to do so
		if (shadowShape && dbgShadowShape)
		{
			shadowShape->debugDraw();
		}

		orderChildren();
		for (unsigned int i=0; i<children.size(); i++)
		{
			// This is the only difference from GameNode::draw(): batchDraw is called
			// on the node's children.
			children[i]->batchDraw();
		}

		glPopMatrix();
	}

	void GameNode::setZOrder(int z)
	{
		if (parent)
			parent->dirtyZOrder = true;
		zOrder = z;
	}

	void GameNode::setShadowShape(Vec2 vertices[], int vertexCount)
	{
		if (shadowShape)
			delete shadowShape;

		shadowShape = new PolygonShape(vertices, vertexCount, this);
	}
	void GameNode::setShadowShapeDebugDraw(bool flag)
	{
		dbgShadowShape = flag;
	}
	PolygonShape* GameNode::getShadowShape()
	{
		return shadowShape;
	}

	/*
	void GameNode::setCollisionShape(Vec2 vertices[], int vertexCount)
	{
		if (colShape)
			delete colShape;

		colShape = new PolygonShape(vertices, vertexCount, this);
	}
	void GameNode::setCollisionShapeDebugDraw(bool flag)
	{
		dbgColShape = flag;
	}

	Vec2 GameNode::validateMovement(Vec2 &o, Vec2 &n)
	{
		return CollisionManager::validateMovement(this, o, n);
	}
	*/
}
