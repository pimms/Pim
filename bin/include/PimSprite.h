#pragma once

#include "PimInternal.h"
#include "pimgamenode.h"

namespace Pim
{
	// Forward declarations
	class SpriteBatchNode;
	class LightingSystem;
	class PolygonShape;
	class SpriteAction;
	class Shader;

	class Sprite : public GameNode
	{
	public:
		Sprite(std::string file);				
		Sprite();								
		virtual ~Sprite();						// Virtual for the sake of subclassing

		void loadSprite(std::string file);		// Load a file

		virtual void draw();					
		virtual void batchDraw();				

		// Perform a single SpriteAction. The Action-object is automatically
		// cleaned after it's execution.
		void runAction(SpriteAction *action);
		void runAction(Action *action);

		// To set the shader, call getShader from ShaderManager and pass it here.
		// The quality is not automatically updated.
		void setShader(Shader *s);
		Shader* getShader() { return shader; }

		// This method is called automatically when adding a sprite to a batch node
		// directly. However, if a sprite is grandchildren of a batch node (or not related
		// to a batch node at all), this method must be called manually.
		void useBatchNode(SpriteBatchNode* batch);

		bool					hidden;			// Hidden?
		Vec2					anchor;			// (0.5,0.5) puts the sprites anchor in the center
		Vec2					scale;			// Scale in X and Y directions. 100% independent.
		Color					color;			// Color overlay
		Rect					rect;			// Used for clipping in sprite sheets

		Shader					*shader;		// The shader

	protected:
		friend class LightingSystem;
		friend class SpriteBatchNode;
		
		GLuint					texID;			// The texture ID
		GLboolean				_a;				// Has the texture alpha?
		png_uint_32				_tw;			// Texture width
		png_uint_32				_th;			// Texture height
		bool					_usebatch;		// Using batch?
	};

}