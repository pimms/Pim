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

		// OLD
		virtual void mouseEvent(MouseEvent &)	{}
		virtual void keyEvent(KeyEvent &)		{}
		virtual void update(float dt)			{}

		// Virtual for the sake of Layers. Override at your own risk.
		virtual Vec2 getWorldPosition();
		virtual float getWorldRotation();

		/*
			TO DO:
			1. Create a Pim::Sprite class
			2. Move all render functionality to that class.
			3. Remove said functionality from this class.
			   This is not a renderable node.
		*/

		// Render self, call render on all children
		virtual void draw();

		float					rotation;		// Rotation - relative to parent
		Vec2					position;		// Position - relative to parent
		std::vector<GameNode*>	children;		// Children

	protected:
		// While it is possible to set the parent variable
		// to some obscure value by yourself, you would be 
		// a giant moron to do so. Leave this alone.
		// No matter what. You do now know what you are doing.
		GameNode	*parent;
	};

}