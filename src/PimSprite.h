#pragma once

#include "PimInternal.h"
#include "pimgamenode.h"

namespace Pim
{
	// Forward declarations
	class SpriteBatchNode;
	class LightingSystem;
	class PolygonShape;
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

		// To set the shader, call getShader from ShaderManager and pass it here.
		// The quality is not automatically updated.
		void setShader(Shader *s);
		Shader* getShader() { return shader; }

		// Set the shadow shape. The vertices __MUST__ be wound counter clockwise,
		// and the shape must be convex.
		void setShadowShape(Vec2 vertices[], int vertexCount);
		void setShadowShapeDebugDraw(bool flag);

		// This method is called automatically when adding a sprite to a batch node
		// directly. However, if a sprite is grandchildren of a batch node, this 
		// method must be called manually.
		void useBatchNode(SpriteBatchNode* batch);

		// Returns the shadow shape. Don't edit the points manually, you savage!
		PolygonShape* getShadowShape();

		bool					hidden;			// Hidden?
		Vec2					anchor;			// (0.5,0.5) puts the sprites anchor in the center
		Vec2					scale;			// Scale in X and Y directions. 100% independent.
		Color					color;			// Color overlay
		Rect					rect;			// Used for clipping in sprite sheets
		bool					dbgShadowShape; // Debug draw the shadow shape?

	protected:
		friend class LightingSystem;
		friend class SpriteBatchNode;
		
		GLuint					texID;			// The texture ID
		GLboolean				_a;				// Has the texture alpha?
		png_uint_32				_tw;			// Texture width
		png_uint_32				_th;			// Texture height
		bool					_usebatch;		// Using batch?

		Shader					*shader;		// The shader
		
		PolygonShape			*shadowShape;	// The shadow casting shape
	};

}