#include "Stdafx.h"
#include "Pim.h"

namespace Pim
{

	Sprite::Sprite(std::string file)
	{
		anchor		= Vec2(0.5f, 0.5f);
		scale		= Vec2(1.f, 1.f);
		color		= Color(1.f, 1.f, 1.f, 1.f);
		texture		= NULL;

		loadSprite(file);
	}
	Sprite::Sprite()
	{
		anchor		= Vec2(0.5f, 0.5f);
		scale		= Vec2(1.f, 1.f);
		color		= Color(1.f, 1.f, 1.f, 1.f);
		texture		= NULL;
	}
	Sprite::~Sprite()
	{
		if (texture)
			free(texture);
	}

	void Sprite::loadSprite(std::string file)
	{
		png_structp		png_ptr;
		png_infop		info_ptr;
		unsigned int	sig_read = 0;
		FILE *fp;

		PimAssert((fp = fopen(file.c_str(), "rb")), file.append(": Does not exist!"));
		
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
		std::cout<<"Image dimensions: " <<_tw <<" x " <<_th <<"\n";

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

		// Release old texture
		if (texture)
			free(texture);

		// Allocate the texture
		unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
		texture = (GLubyte*)malloc(_th * row_bytes);

		// Read the image into the texture
		png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

		for (int i=0; i<_th; i++)
		{
			memcpy(texture+(row_bytes * (_th-1-i)), row_pointers[i], row_bytes);
		}

		// Cleanup
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		
		// Default rect is the texture size. Crop at free!
		rect.width = _tw;
		rect.height = _th;
	}

	void Sprite::draw()
	{
		glPushMatrix();

		// Update view matrix
		glTranslatef(position.x, position.y, 0.f);
		glRotatef(rotation, 0.f, 0.f, 1.f);

		// Push matrix before scaling
		glPushMatrix();
		glScalef(scale.x, scale.y, 1.f);

		
		// The loading is complete, now let's bind some textures!
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, _a?4:3, _tw, _th, 0, _a?GL_RGBA:GL_RGB, GL_UNSIGNED_BYTE, texture);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

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
		

		// Children are unaffected by their parent's scale. Restore.
		glPopMatrix();

		for (unsigned int i=0; i<children.size(); i++)
		{
			children[i]->draw();
		}

		// Restore this parent's view matrix
		glPopMatrix();
	}
}