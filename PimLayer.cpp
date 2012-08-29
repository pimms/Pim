#include "StdAfx.h"
#include "PimLayer.h"

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

	void Layer::immovableLayer(bool immov)
	{
		PimAssert(parent != NULL && !isTopLayer, "Error: cannot set orphan layer as immovable.");
		immovable = immov; 
	}

	void Layer::_topLevelNode()
	{
		PimAssert(!topLayer, "Error: another top layer already exist!");
		isTopLayer	= true;
		topLayer	= this;
	}
}