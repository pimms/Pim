#include "StdAfx.h"
#include "PimLayer.h"

#include "Pim.h"
#include "PimVec2.h"
#include "PimGameControl.h"
#include "PimException.h"

namespace Pim
{
	Layer* Layer::topLayer = NULL;

	Layer::Layer(void)
	{
		isTopLayer = false;
		color = Color(1.f, 1.f, 1.f, 1.f);
	}
	Layer::~Layer(void)
	{
		if (isTopLayer)
			topLayer = NULL;
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
		glTranslatef(position.x, position.y, 0.f);
		glRotatef(rotation, 0.f, 0.f, 1.f);

		for (unsigned int i=0; i<children.size(); i++)
		{
			children[i]->draw();
		}

		glPopMatrix();
	}

	void Layer::_topLevelNode()
	{
		PimAssert(!topLayer, "Error: another top layer already exist!");
		isTopLayer	= true;
		topLayer	= this;
	}
}