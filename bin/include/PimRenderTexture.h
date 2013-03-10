#pragma once

#include "PimInternal.h"
#include "PimVec2.h"

namespace Pim {
	/**
	 @class 		RenderTexture
	 @brief 		Wrapper around a frame-buffer object.
	 @details 		The class uses a FBO in combination with an RBO (can be enabled
	 				in the constructor - disabled by default) to accommodate
	 				advanced rendering to textures.
	 
	 				Used by Layer when a Shader is used, and by LightingSystem.
	 */
	
	class Vec2;

	class RenderTexture {
	public:
		bool			retainTexture;
		
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
	
	/**
	 @var 			retainTexture
	 @details 		If false, the texture is deleted along with the object.
	 				Otherwise, the texture is kept alive, but you must manage
	 				it yourself.
	 */
	
	/**
	 @fn			BindFBO
	 @brief 		The FBO will capture all OpenGL render-calls.
	 @details 		Call this method at the start of your to-texture rendering.
	 */
	
	/**
	 @fn 			UnbindFBO
	 @brief 		The FBO will be unbound, thus uncapturing OpenGL render-calls.
	 @details 		Call when you are done rendering to the texture.
	 */
	
	/**
	 @fn 			Clear
	 @brief 		Clear the color buffer with the flag GL_COLOR_BUFFER_BIT.
	 @param 		additional
	 				Any flags in addition to GL_COLOR_BUFFER_BIT - for instance,
	 				GL_STENCIL_BUFFER_BIT, etc.
	 */
	
	/**
	 @fn 			BindTex
	 @brief 		Bind the texture.
	 */
	
	/**
	 @fn 			UnbindTex
	 @brief 		Unbind the current GL_TEXTURE_2D.
	 */
}