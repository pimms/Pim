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

#include <vector>

#include "PimVec2.h"

namespace Pim
{
	// Forward declarations
	class Input;

	class GameNode
	{
	public:
		GameNode();
		~GameNode();
		
		// For comparison only. 
		const GameNode* getParent() { return parent; }

		void addChild(GameNode *ch);
		void removeChild(GameNode *ch, bool cleanup=false);
		void removeAllChildren(bool cleanup=false);

		void listenInput();
		void unlistenInput();
		void listenFrame();
		void unlistenFrame();

		virtual void mouseEvent(Input *in)	{}
		virtual void keyEvent(Input *in)	{}
		virtual void update(float dt)		{}

		// Virtual for the sake of Layers. Override at your own risk.
		virtual Vec2 getWorldPosition();

		// Render self, call render on all children
		virtual void draw();

		float					rotation;
		Vec2					position;
		std::vector<GameNode*>	children;

	protected:
		// While it is possible to set the parent variable
		// to some obscure value by yourself, you would be 
		// a giant moron to do so. Leave this alone.
		// No matter what.
		GameNode	*parent;
	};

}