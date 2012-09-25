#pragma once
#include "pimsprite.h"
#include "Stdafx.h"

/*
	SpriteBatchNode is the best way to go if you're using a huge sprite sheet.
	The batch node loads the sprite sheet into memory once, and all (sprite) children
	attached to it will only be able to use cutouts / all of the batch node texture.

	Note that granchildren of a batch node also are required to use the batch node 
	texture. They cannot load a texture of their own. Use a separate batch node
	for this.

	The batch node itself does not care what position it is drawn in. It uses it's
	parent's position as a null point, and children of the batch node will be drawn
	relative to their grandparent's position.

	Note that layer's should never be added as child to a batch node. It may work, but
	it's not officially supported. At all.
*/

namespace Pim
{

	class SpriteBatchNode : public Sprite
	{
	public:
		SpriteBatchNode(std::string file);
		~SpriteBatchNode(void);

		void draw();
		void batchDraw();

	private:
		SpriteBatchNode() {}
	};

}