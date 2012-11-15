#include "PimRenderTexture.h"

namespace Pim
{
	RenderTexture::RenderTexture(Vec2 resolution, bool renderBuffer)
	{
		res = resolution;
		fbo = 0;
		rbo = 0;
		tex = 0;

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, (GLsizei)res.x, (GLsizei)res.y, 
						0, GL_RGBA, GL_FLOAT, NULL);

		// Create the main framebuffer
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

		if (renderBuffer)
		{
			// Create the renderbuffer
			glGenRenderbuffers(1, &rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);

			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 
						(GLsizei)res.x, (GLsizei)res.y);

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
						GL_RENDERBUFFER, rbo);

			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	RenderTexture::~RenderTexture()
	{
		if (fbo)
			glDeleteFramebuffers(1, &fbo);
		if (rbo)
			glDeleteRenderbuffers(1, &rbo);
		if (tex)
			glDeleteTextures(1, &tex);
	}

	void RenderTexture::bindFBO()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glPushAttrib(GL_VIEWPORT_BIT);
		glViewport(0,0, (GLsizei)res.x, (GLsizei)res.y);
		glOrtho(0, res.x, 0, res.y, 0, 1);
	}
	void RenderTexture::unbindFBO()
	{
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glPopAttrib();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void RenderTexture::clear(GLenum additional)
	{
		glClear(GL_COLOR_BUFFER_BIT | additional);
	}

	void RenderTexture::bindTex()
	{
		glBindTexture(GL_TEXTURE_2D, tex);
	}
	void RenderTexture::unbindTex()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint RenderTexture::getTex()
	{
		return tex;
	}
}
