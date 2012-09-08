#pragma once

#include "Stdafx.h"
#include "pimgamenode.h"

namespace Pim
{
	// Forward declarations
	class SpriteBatchNode;

	class Sprite : public GameNode
	{
	public:
		Sprite(std::string file);				
		Sprite();								
		virtual ~Sprite();						// Virtual for the sake of subclassing

		void loadSprite(std::string file);		// Load a file

		virtual void draw();					
		virtual void batchDraw();				

		void useBatchNode(SpriteBatchNode* batch);

		Vec2					anchor;			// (0.5,0.5) puts the sprites anchor in the center
		Vec2					scale;			// Scale in X and Y directions. 100% independent.
		Color					color;			// Color overlay
		Rect					rect;			// Used for clipping in sprite sheets

	protected:
		GLubyte					*texture;		// The texture data
		GLboolean				_a;				// Has the texture alpha?
		png_uint_32				_tw;			// Texture width
		png_uint_32				_th;			// Texture height
	};

}