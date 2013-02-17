#pragma once

#include "PimInternal.h"
#include "pimgamenode.h"

namespace Pim {
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

	protected:

		GLuint					texID;			// The texture ID
		GLboolean				_a;				// Has the texture alpha?
		png_uint_32				_tw;			// Texture width
		png_uint_32				_th;			// Texture height
		bool					_usebatch;		// Using batch?
		const SpriteBatchNode*	_batchNode;		// The batch node used
	};

}