#include "StdAfx.h"

#include "Pim.h"
#include "PimGameNode.h"
#include "PimException.h"
#include "PimInput.h"

namespace Pim
{

	GameNode::GameNode()
	{
		parent = NULL;
	}
	GameNode::~GameNode()
	{
		removeAllChildren(true);
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
		GameControl::getSingleton()->addInputListener(this);
	}
	void GameNode::unlistenInput()
	{
		GameControl::getSingleton()->removeInputListener(this);
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

	void GameNode::draw()
	{
		// render self

		for (unsigned int i=0; i<children.size(); i++)
		{
			children[i]->draw();
		}
	}
}
