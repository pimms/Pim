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

	void Layer::_topLevelNode()
	{
		PimAssert(!topLayer, "Error: another top layer already exist!");
		isTopLayer	= true;
		topLayer	= this;
	}
}