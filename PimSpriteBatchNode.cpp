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

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, _a?4:3, _tw, _th, 0, _a?GL_RGBA:GL_RGB, GL_UNSIGNED_BYTE, texture);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		orderChildren();
		for (unsigned int i=0; i<children.size(); i++)
		{
			children[i]->batchDraw();
		}

		glPopMatrix();
	}
}