#include "Stdafx.h"

#include "PimVec2.h"
#include "PimSprite.h"
#include "PimException.h"
#include "PimShaderManager.h"
#include "PimPolygonShape.h"
#include "PimGameControl.h"
#include "PimSpriteBatchNode.h"

namespace Pim
{

	Sprite::Sprite(std::string file)
	{
		anchor			= Vec2(0.5f, 0.5f);
		scale			= Vec2(1.f, 1.f);
		color			= Color(1.f, 1.f, 1.f, 1.f);
		texID			= -1;
		shader			= NULL;
		shadowShape		= NULL;
		dbgShadowShape	= false;
		hidden			= false;

		loadSprite(file);
	}
	Sprite::Sprite()
	{
		anchor			= Vec2(0.5f, 0.5f);
		scale			= Vec2(1.f, 1.f);
		color			= Color(1.f, 1.f, 1.f, 1.f);
		texID			= -1;
		shader			= NULL;
		shadowShape		= NULL;
		dbgShadowShape	= false;
		hidden			= false;
	}
	Sprite::~Sprite()
	{
		glDeleteTextures(1, &texID);

		if (shadowShape)
			delete shadowShape;
	}

	void Sprite::loadSprite(std::string file)
	{
		png_structp		png_ptr;
		png_infop		info_ptr;
		unsigned int	sig_read = 0;
		FILE *fp;

		fopen_s(&fp, file.c_str(), "rb");
		PimAssert(fp != NULL, file.append(": Does not exist!"));
		
		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

		if (!png_ptr)
		{
			fclose(fp);
			PimAssert(false, "Error: failed instantiating png reading");
		}

		info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr)
		{
			fclose(fp);
			PimAssert(false, "Error: failed instantioating png info");
		}

		if (setjmp(png_jmpbuf(png_ptr)))
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			fclose(fp);
			PimAssert(false, "Error in loading png: something went wrong.");
		}

		// Init complete, init for real
		png_init_io(png_ptr, fp);
		png_set_sig_bytes(png_ptr, sig_read);
		png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);


		// Get the size
		_tw = png_get_image_width(png_ptr, info_ptr);
		_th = png_get_image_height(png_ptr, info_ptr);

		// Get the color format
		png_byte byte = png_get_color_type(png_ptr, info_ptr);
		switch (byte)
		{
			case PNG_COLOR_TYPE_RGB:
				_a = false;
				break;
			case PNG_COLOR_TYPE_RGBA:
				_a = true;
				break;
			default:
				PimAssert(false,"Image format not supported");
		}

		// Allocate the texture
		unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
		GLubyte *texture = (GLubyte*)malloc(_th * row_bytes);

		// Read the image into the texture
		png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

		for (unsigned int i=0; i<_th; i++)
		{
			memcpy(texture+(row_bytes * (_th-1-i)), row_pointers[i], row_bytes);
		}

		// Cleanup
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		
		// Default rect is the texture size. Crop at free!
		rect.width = _tw;
		rect.height = _th;

		// Create the texture
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);

		// The loading is complete, now let's bind some textures!
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, _a?4:3, _tw, _th, 0, _a?GL_RGBA:GL_RGB, 
					 GL_UNSIGNED_BYTE, texture);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		free(texture);
	}

	void Sprite::draw()
	{
		glPushMatrix();

		// Update view matrix
		Vec2 fac = GameControl::getSingleton()->coordinateFactor();

		if (allowMidPixelPosition)
			glTranslatef(position.x / fac.x, position.y / fac.y, 0.f);
		else
			glTranslatef(floor(position.x) / fac.x, floor(position.y) / fac.y, 0.f);

		glRotatef(rotation, 0.f, 0.f, 1.f);

		// Push matrix before scaling and overlaying color
		glPushMatrix();

		fac = GameControl::getSingleton()->windowScale();
		glScalef(scale.x * fac.x, scale.y * fac.y, 1.f);
		glColor4f(color.r, color.g, color.b, color.a);

		glBindTexture(GL_TEXTURE_2D, texID);

		if (shader)
		{
			shader->setUniform1i("texture", 0);
			glUseProgram(shader->getProgram());
		}

		if (!hidden)
		{
			// Counter clockwise 
			glBegin(GL_QUADS);
				glTexCoord2f((float)rect.x / (float)_tw, (float)rect.y / (float)_th);
				glVertex2f(-anchor.x * rect.width, -anchor.y * rect.height);

				glTexCoord2f((float)rect.x/(float)_tw + (float)rect.width/(float)_tw, (float)rect.y / (float)_th);
				glVertex2f((1.f-anchor.x) * rect.width, -anchor.y * rect.height);

				glTexCoord2f((float)rect.x/(float)_tw + (float)rect.width/(float)_tw, (float)rect.y/(float)_th + (float)rect.height/(float)_th);
				glVertex2f((1.f-anchor.x) * rect.width, (1.f-anchor.y) * rect.height);

				glTexCoord2f((float)rect.x/(float)_tw, (float)rect.y/(float)_th + (float)rect.height/(float)_th);
				glVertex2f(-anchor.x * rect.width, (1.f-anchor.y) * rect.height);
			glEnd();
		}
		
		glUseProgram(0);

		// Debug draw shadow shape if flagged to do so
		if (shadowShape && dbgShadowShape)
			shadowShape->debugDraw();

		// Debug draw the collison shape if required
		if (colShape && dbgColShape)
			colShape->debugDraw();

		// Children are unaffected by their parent's scale. Restore.
		glPopMatrix();

		orderChildren();
		for (unsigned int i=0; i<children.size(); i++)
		{
			children[i]->draw();
		}

		// Restore this parent's view matrix
		glPopMatrix();
	}
	void Sprite::batchDraw()
	{
		glPushMatrix();

		// Update view matrix
		Vec2 fac = GameControl::getSingleton()->coordinateFactor();

		if (allowMidPixelPosition)
			glTranslatef(position.x / fac.x, position.y / fac.y, 0.f);
		else
			glTranslatef(floor(position.x) / fac.x, floor(position.y) / fac.y, 0.f);

		glRotatef(rotation, 0.f, 0.f, 1.f);

		// Push matrix before scaling
		glPushMatrix();

		fac = GameControl::getSingleton()->windowScale();
		glScalef(scale.x * fac.x, scale.y * fac.y, 1.f);
		glColor4f(color.r, color.g, color.b, color.a);

		if (shader)
		{
			shader->setUniform1i("texture", 0);
			glUseProgram(shader->getProgram());
		}

		if (!hidden)
		{
			glBegin(GL_QUADS);
				// Counter clockwise winding, beginning in bottom left corner //
				glTexCoord2f((float)rect.x / (float)_tw, (float)rect.y / (float)_th);
				glVertex2f(-anchor.x * rect.width, -anchor.y * rect.height);

				glTexCoord2f((float)rect.x/(float)_tw + (float)rect.width/(float)_tw, (float)rect.y / (float)_th);
				glVertex2f((1.f-anchor.x) * rect.width, -anchor.y * rect.height);

				glTexCoord2f((float)rect.x/(float)_tw + (float)rect.width/(float)_tw, (float)rect.y/(float)_th + (float)rect.height/(float)_th);
				glVertex2f((1.f-anchor.x) * rect.width, (1.f-anchor.y) * rect.height);

				glTexCoord2f((float)rect.x/(float)_tw, (float)rect.y/(float)_th + (float)rect.height/(float)_th);
				glVertex2f(-anchor.x * rect.width, (1.f-anchor.y) * rect.height);
			glEnd();
		}

		glUseProgram(0);

		// Debug draw shadow shape if flagged to do so
		if (shadowShape && dbgShadowShape)
			shadowShape->debugDraw();

		// Debug draw the collison shape if required
		if (colShape && dbgColShape)
			colShape->debugDraw();

		// Children are unaffected by their parent's scale. Restore.
		glPopMatrix();

		orderChildren();
		for (unsigned int i=0; i<children.size(); i++)
		{
			children[i]->batchDraw();
		}

		// Restore this parent's view matrix
		glPopMatrix();
	}

	void Sprite::useBatchNode(SpriteBatchNode *batch)
	{
		// Assign alpha and texture dimensions
		_a    = batch->_a;
		_tw   = batch->_tw;
		_th	  = batch->_th;
		texID = batch->texID;
	}

	void Sprite::setShader(Shader *s)
	{
		shader = s;
	}

	void Sprite::setShadowShape(Vec2 vertices[], int vertexCount)
	{
		if (shadowShape)
			delete shadowShape;

		shadowShape = new PolygonShape(vertices, vertexCount, this);
	}
	void Sprite::setShadowShapeDebugDraw(bool flag)
	{
		dbgShadowShape = flag;
	}
	PolygonShape* Sprite::getShadowShape()
	{
		return shadowShape;
	}
}