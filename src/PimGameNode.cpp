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
		parent		= NULL;
		rotation	= 0.f;
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

	void GameNode::draw()
	{
		glPushMatrix();

		// Update position
		glTranslatef(position.x, position.y, 0.f);
		//glTranslatef(getWorldPosition().x, getWorldPosition().y, 0.f);
		glRotatef(rotation, 0.f, 0.f, 1.f);

		for (unsigned int i=0; i<children.size(); i++)
		{
			children[i]->draw();
		}

		glPopMatrix();
	}
}
