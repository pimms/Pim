#pragma once

#include "PimInternal.h"
#include "PimVec2.h"

namespace Pim {
	class Vec2;

	class RenderTexture {
	public:
		bool			retainTexture;	// Retain texture after object's death?

						RenderTexture(const Vec2 resolution, const bool renderBuffer=false);
						~RenderTexture();
		void			BindFBO() const;
		void			UnbindFBO() const;
		void			Clear(const GLenum additional=0) const;
		void			BindTex() const;
		void			UnbindTex() const;
		GLuint			GetTex() const;

	private:
		Vec2			res;
		GLuint			fbo;
		GLuint			rbo;
		GLuint			tex;

						RenderTexture() {}
						RenderTexture(const RenderTexture&) {}
	};
}