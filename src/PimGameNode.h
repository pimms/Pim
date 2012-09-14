#pragma once
/*
	GameNode is the base node used in pim.

	GameNode in itself is not renderable. Use Pim::Sprite for images. GameNodes
	can however draw natively using OpenGL by itself.

	GameNodes can be created at will. However, for a GameNode to be rendered, it 
	MUST be added to a parent (another game node). Most nodes will be attached
	directly to the Layer.

	A node can only be attached to one parent at a time, and an exception will be 
	thrown if a node with a parent is attempted added to a node.

	Nodes are deleted automatically if "true" is passed to removeChild(GameNode,bool), 
	or by passing "true" to removeAllChildren(bool), or upon the parent's deletion.
*/

#include "Stdafx.h"

#include "PimVec2.h"

namespace Pim
{
	// Forward declarations
	class Vec2;
	class Input;
	class MouseEvent;
	class KeyEvent;

	class GameNode
	{
	public:
		GameNode();
		virtual ~GameNode();
		
		// For comparison only. 
		const GameNode* getParent() { return parent; }

		void addChild(GameNode *ch);
		void removeChild(GameNode *ch, bool cleanup=false);
		void removeAllChildren(bool cleanup=false);

		void listenInput();
		void unlistenInput();

		void listenKeys();
		void unlistenKeys();

		void listenMouse();
		void unlistenMouse();

		void listenFrame();
		void unlistenFrame();

		virtual void mouseEvent(MouseEvent &)	{}
		virtual void keyEvent(KeyEvent &)		{}
		virtual void update(float dt)			{}

		// Virtual for the sake of Layers.
		virtual Vec2 getWorldPosition();	
		virtual float getWorldRotation();	

		// Returns the position of this node relative to the first layer-parent.
		// Returns (0,0) from ALL layers.
		virtual Vec2 getLayerPosition();

		// Sorts children based on their zOrder
		void orderChildren();

		// Render self, call draw on all children
		virtual void draw();

		// Render self, call batchDraw on all children.
		// The only difference is that the texture has already been loaded into
		// openGL, as the node (sprite) has no texture of it's own.
		virtual void batchDraw();

		// Change the ZOrder (0 is drawn first)
		void setZOrder(int z);


		float					rotation;		// Rotation - relative to parent
		Vec2					position;		// Position - CCW degrees - relative to parent
		std::vector<GameNode*>	children;		// Children

		// If this is false, nodes will be placed at the integer equivalent of their position.
		// For instance, if the renderResolution of the window is 10x10, and the nodes position
		// is [3.4,2.1], the ACTUAL position of the node will be [3,2]. 
		// It is adviced to leave this off for layers, as the scrolling will be VERY jagged,
		// even if you try to simulate that fancy retro 8-bit style.
		// Best practice: true for sprites, false for layers.
		bool					allowMidPixelPosition;

	protected:
		// While it is possible to set the parent variable
		// to some obscure value by yourself, you would be 
		// a giant moron to do so. Leave this alone.
		// No matter what. You do now know what you are doing.
		GameNode	*parent;

		// zOrder is handled in an ascending fashion: 0 is drawn first, then 1, etc.
		// The ordering only occurs with siblings of the same parent. 
		// If two sprites with ZO 0 and 1 are drawn, ZO1 will overlap ZO0, as ZO0 is drawn first.
		// Use setZOrder(int) to change the order - this will dirtien the parent.
		int						zOrder;

		// If dirtyZOrder is false, orderChildren() will do nothing.
		// The z-order is dirty when a new child has been added, and is 
		// clean when the children are ordered.
		bool		dirtyZOrder;
	};

}