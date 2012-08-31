#pragma once
/*
	The Layer is the "top level" controller of your game, which is customizeable
	by you. It acts like a regular GameNode in terms of children and parent relationship,
	and multiple Layers can be added to your screen at any given time. For instance,
	the most sensible thing to do is attach all user interface objects on one layer,
	and have the game visuals on another.

	IMPORTANT NOTICE ABOUT LAYER RELATIONSHIPS AND UI!
	A Layer containing game objects is movable. I.e., you can move the layer opposite of
	the direction the player is moving, thus "scrolling the world". In order for any UI-layers
	to stand completely still, you NEED to flag the Layer as an 'immovableLayer'. This will cause
	the "getWorldPosition()" method to return (0,0)+(position) no matter what. 
	Failure to flag immovable will cause the same method to return 
	(parent->worldPosition())+(position) - which effectively would scroll the UI along with the
	world. Any sub-layers to the UI layer can be treated however you'd like, as the UI layer
	will return a constant value regardless of it's parent.

	As the top level Layer will not have a parent, it is flagged as "topLayer", and a 
	static pointer is set to point to this layer. It can be accessed through the static method
	Layer::getTopLayer().

	There must always be a top layer. Even if you were to strike down the top layer, a new
	top layer must take it's place. This is handled automatically by the GameController
	when you attach a new Layer to it.
*/

#include "PimVec2.h"
#include "PimGameNode.h"

namespace Pim
{
	// Forward declarations
	class GameControl;
	struct Color;

	class Layer : public GameNode
	{
	public:
		Layer(void);
		virtual ~Layer(void);				// Virtual for the sake of subclassing

		static Layer* getTopLayer();

		Vec2 getWorldPosition();			
		float getWorldRotation();			

		void immovableLayer(bool immov);

		void draw();

		const Color getColor() { return color; }

		// USE loadResources TO INSTANTIATE YOUR LAYER - DO NOT USE Layer()!
		virtual void loadResources() {}

	protected:
		friend class GameControl;

		// Called when attached to the GameControl as the top level node
		void _topLevelNode();

		bool			isTopLayer;
		bool			immovable;

		Color			color;

		static Layer* topLayer;
	};

}