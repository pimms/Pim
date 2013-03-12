#pragma once

#include "PimInternal.h"
#include "pimgamenode.h"

namespace Pim {
	/**
	 @class 		Sprite
	 @brief 		A renderable object that may use a PNG file to initiate a 
	 				texture.
	 @details 		In most cases, this is the most utilized class in the node-tree.
	 				
	 				Sprites only support the PNG format (0.9).
	 
	 				If you intend to reuse the same texture at several places,
	 				consider loading the texture once via a SpriteBatchNode, 
	 				and call @e UseBatchNode on your relevant Sprite objects.
	 				
	 				Note that if you're using a sprite sheet, you may want to
	 				set the clipping-rectangle (the @e rect attribute) to 
	 				fit the frame you want from your sheet.
	 
	 				The coordinate system used in textures is as general in Pim:
	 				[0,0] is in the bottom left corner, [w,h] is in the top right
	 				corner.
	 */
	
	class SpriteBatchNode;
	class LightingSystem;
	class PolygonShape;
	class SpriteAction;
	class Shader;

	class Sprite : public GameNode {
	protected:
		friend class LightingSystem;
		friend class SpriteBatchNode;

	public:
		bool					hidden;			// Hidden?
		Vec2					anchor;			// (0.5,0.5) puts the sprites anchor in the center
		Vec2					scale;			// Scale in X and Y directions. 100% independent.
		Color					color;			// Color overlay
		Rect					rect;			// Used for clipping in sprite sheets
		Shader*					shader;		

								Sprite(string file);
								Sprite();
		virtual					~Sprite();
		void					LoadSprite(string file);
		virtual void			Draw();
		virtual void			BatchDraw();
		void					RunAction(SpriteAction *action);
		void					RunAction(Action *action);
		void					SetShader(Shader *s);
		Shader*					GetShader() const;
		void					UseBatchNode(const SpriteBatchNode* batch);
		virtual void			ReloadTextures();

	protected:

		string					textureFile;
		GLuint					texID;			// The texture ID
		GLboolean				_a;				// Has the texture alpha?
		png_uint_32				_tw;			// Texture width
		png_uint_32				_th;			// Texture height
		bool					_usebatch;		// Using batch?
		const SpriteBatchNode*	_batchNode;		// The batch node used
	};
	
	
	/**
	 @fn 			LoadSprite
	 @brief 		Load a PNG file to a texture
	 */
	
	/**
	 @fn 			SetShader
	 @brief 		Set a shader that will be used when the Sprite is drawn.
	 */
	
	/**
	 @fn 			UseBatchNode
	 @brief 		Use the texture of the provided SpriteBatchNode.
	 @details 		Smart to use when you use the same sprite or texture at 
	 				multiple points in your game. 
	 
	 				Note that unless the Sprite is added as child to the BatchNode
	 				(in which case this method is called automatically), you do
					not gain the performance boost normally gained when adding
	 				Sprites to batch nodes, as the Sprite must still bind the
	 				texture of the batch node in order to draw it.
	 */
	
	/**
	 @fn 			ReloadTextures
	 @brief 		Reload the textures
	 @details 		If the Sprite has a texture, and it does not use a batch node,
	 				the PNG file is reloaded into the texture attribute.
	 
	 				As mentioned other places, the entire OpenGL-context is deleted
	 				upon window resize under OSX.
	 */

}