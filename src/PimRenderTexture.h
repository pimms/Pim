#pragma once

#include "PimInternal.h"
#include "PimVec2.h"

namespace Pim
{
	// Forward declarations
	class Vec2;

	class RenderTexture
	{
	public:
		RenderTexture(Vec2 resolution, bool renderBuffer=false);
		~RenderTexture();

		void bindFBO();
		void unbindFBO();

		void clear(GLenum additional=0);

		void bindTex();
		void unbindTex();

		GLuint getTex();

		// If this is true, the texture will not be deleted 
		// with the RenderTexture.
		bool retainTexture;	

	private:
		RenderTexture() {}
		RenderTexture(const RenderTexture&) {}

		Vec2		res;

		GLuint		fbo;
		GLuint		rbo;
		GLuint		tex;
	};
}