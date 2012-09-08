#include "Stdafx.h"

#include "Pim.h"
#include "PimSpriteBatchNode.h"

#include <functional>

namespace Pim
{
	SpriteBatchNode::SpriteBatchNode(std::string file)
		: Sprite(file)
	{
		// Using Pim::Sprite's init method
	}
	SpriteBatchNode::~SpriteBatchNode(void)
	{
		// Using Pim::Sprite's destruction method
	}

	void SpriteBatchNode::draw()
	{
		batchDraw();
	}
	void SpriteBatchNode::batchDraw()
	{
		glPushMatrix();

		glBindTexture(GL_TEXTURE_2D, texID);

		orderChildren();
		for (unsigned int i=0; i<children.size(); i++)
		{
			children[i]->batchDraw();
		}

		glPopMatrix();
	}
}