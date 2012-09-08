#include "StdAfx.h"

#include "Pim.h"
#include "PimVec2.h"
#include "PimGameNode.h"
#include "PimException.h"
#include "PimInput.h"

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
	}
	GameNode::~GameNode()
	{
		removeAllChildren(true);
		
		unlistenFrame();
		unlistenInput();
	}

	void GameNode::addChild(GameNode *ch)
	{
		PimAssert(!ch->getParent(), "Node already has a parent");

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
				return;
			}
		}
	}
	void GameNode::removeAllChildren(bool cleanup)
	{
		// Delete all if required
		if (cleanup)
		{
			for (unsigned int i=0; i<children.size(); i++)
			{
				delete children[i];
			}
		}

		// THEN clear the array
		children.clear();
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

	void GameNode::orderChildren()
	{
		if (!dirtyZOrder || children.size() < 2)
			return;
		
		// Insertion sorting - the children should be somewhat sorted already.
		int i;
		GameNode *key;
		for (unsigned int j=1; j<children.size(); j++)
		{
			key = children[j];
			i = j - 1;
			std::cout<<i <<"\n";
			while (i >= 0 && children[i]->zOrder > key->zOrder)
			{
				children[i+1] = children[i];
				i--;
			}
			children[i+1] = key;
		}

		std::cout<<"\n";

		dirtyZOrder = false;
	}

	void GameNode::draw()
	{
		glPushMatrix();

		// Update position
		Vec2 fac = GameControl::getSingleton()->windowScale();

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

		glPopMatrix();
	}
	void GameNode::batchDraw()
	{
		glPushMatrix();

		// Update position
		Vec2 fac = GameControl::getSingleton()->windowScale();

		if (allowMidPixelPosition)
			glTranslatef(position.x / fac.x, position.y / fac.y, 0.f);
		else
			glTranslatef(floor(position.x) / fac.x, position.y / fac.y, 0.f);

		glRotatef(rotation, 0.f, 0.f, 1.f);

		orderChildren();
		for (unsigned int i=0; i<children.size(); i++)
		{
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
}
