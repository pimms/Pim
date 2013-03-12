#pragma once
#include "pimsprite.h"
#include "PimInternal.h"

namespace Pim {
	/**
	 @class 		SpriteBatchNode
	 @brief 		Loads a texture into memory once, and it is used by all
					Sprite children of this node.
	 @details 		You need not add Sprites to a batch node for the Sprites
	 				to use the batch node texture. You may create the 
	 				batch node once in your layer (and add it to the layer so
	 				you don't have to delete it yourself), and simply tell 
	 				your Sprites to use the batch node's texture as such:
	 
	 				Note that in option 2, the performance gained from not having
	 				to rebind the texture for each draw call is lost.
	 
	 @code
	 				SpriteBatchNode *bn = new SpriteBatchNode("a.png");
					AddChild(bn);
	 
	 				// Option1:
	 				bn->AddChild(playerSprite);
	 
	 				// Option2:
	 				AddChild(playerSprite);
	 				playerSprite->UseBatchNode(bn);
	 @endcode
	 */
	
	class SpriteBatchNode : public Sprite {
	public:
					SpriteBatchNode(string file);
					SpriteBatchNode();
					~SpriteBatchNode(void);
		void		AddChild(GameNode *ch);
		void		Draw();
		void		BatchDraw();
	};
}