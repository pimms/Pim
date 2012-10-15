#include "Stdafx.h"
#include "PimFont.h"
#include "PimLabel.h"
#include "PimGameControl.h"
#include "PimHelperFunctions.h"

namespace Pim
{
	Font::Font(std::string font, int psize, bool bilinearFiltering)
	{
		charWidth	= NULL;
		tex			= NULL;
		size		= psize;
		filter		= bilinearFiltering;
		init(font, size);
	}
	Font::~Font(void)
	{
		clean();
	}

	int Font::getCharacterWidth(const char ch)
	{
		if (charWidth)
			return (int)charWidth[ch];
#ifdef _DEBUG
		std::cout<<"WARNING: Font uninitalized!\n";
#endif
		return 0;
	}

	void Font::init(std::string font, int size)
	{
		FT_Error error;
		FT_Library libFT = NULL;
		FT_Face face = NULL;

		error = FT_Init_FreeType(&libFT);
		if (error)
		{
			MessageBox(
				NULL, 
				"Could not initialize FreeType.", 
				"FreeType error!", 
				MB_ICONEXCLAMATION|MB_OK);
			return;
		}

		error = FT_New_Face(libFT, font.c_str(), 0, &face);
		if (error == FT_Err_Unknown_File_Format)
		{
			std::string errstr = "Could not recognize format of file:\n";
			errstr.append(font);
			MessageBox(
				NULL,
				errstr.c_str(),
				"FreeType error!",
				MB_ICONEXCLAMATION|MB_OK);
			return;
		} 
		else if (error) 
		{
			std::string errstr = "Unkown error opening file:\n";
			errstr.append(font);
			MessageBox(
				NULL,
				errstr.c_str(),
				"FreeType error!",
				MB_ICONEXCLAMATION|MB_OK);
			return;
		}

		error = FT_Set_Char_Size(
			face,
			size << 6,
			size << 6,
			96,
			96 );
		if (error)
		{
			MessageBox(
				NULL,
				"Unable to set the character size.",
				"Freetype error!",
				MB_ICONEXCLAMATION | MB_OK);
			return;
		}

		renderToTexture(face);

		FT_Done_Face(face);
		FT_Done_FreeType(libFT);
	}

	void Font::renderToTexture(FT_Face face)
	{
		// Clean up if the textures already exist
		clean();

		// Begin rendering the bitmaps to the textures
		tex = new GLuint[128];
		charWidth = new unsigned char[128];

		listBase = glGenLists(128);
		glGenTextures(128, tex);

		for (unsigned char i=0; i<128; i++)
			createDisplayList(face, i);
	}
	void Font::createDisplayList(FT_Face face, char ch)
	{
		if (FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), FT_LOAD_DEFAULT | FT_LOAD_MONOCHROME))
		{
			MessageBox(
				NULL,
				"Error: FT_Load_Glyph failed!",
				"FreeType error!",
				MB_ICONEXCLAMATION | MB_OK );
			return;
		}

		FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		FT_Bitmap bitmap = face->glyph->bitmap;

		int width = nextPow2(bitmap.width);
		int height = nextPow2(bitmap.rows);

		GLubyte *expandedData = new GLubyte[2 * width * height];

		for (int j=0; j<height; j++)
		{
			for (int i=0; i<width; i++)
			{
				expandedData[2*(i+j*width)] = expandedData[2*(i+j*width)+1] =
					(i>=bitmap.width || j>=bitmap.rows) ?
					0 : bitmap.buffer[i + bitmap.width*j];
			}
		}

		glBindTexture(GL_TEXTURE_2D, tex[ch]);

		if (filter)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		else 
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
			GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expandedData);

		glNewList(listBase+ch, GL_COMPILE);

		glBindTexture(GL_TEXTURE_2D, tex[ch]);

		glPushMatrix();

		glTranslatef(
			face->glyph->bitmap_left,
			face->glyph->bitmap_top - bitmap.rows,
			0.f);
		
		float x = (float)bitmap.width / (float)width;
		float y = (float)bitmap.rows / (float)height;

		glBegin(GL_QUADS);
			glTexCoord2f(0.f,0.f); glVertex2i(0, bitmap.rows);
			glTexCoord2f(0.f,  y); glVertex2i(0, 0);
			glTexCoord2f(  x,  y); glVertex2i(bitmap.width, 0);
			glTexCoord2f(  x,  0); glVertex2i(bitmap.width, bitmap.rows);
		glEnd();
		glPopMatrix();

		glTranslatef(face->glyph->advance.x >> 6, 0.f, 0.f);

		glEndList();

		charWidth[ch] = face->glyph->advance.x >> 6;

		delete[] expandedData;
	}

	void Font::clean()
	{
		if (tex)
		{
			glDeleteLists(listBase, 128);
			glDeleteTextures(128, tex);
			delete[] tex;
			delete[] charWidth;
		}
	}
}