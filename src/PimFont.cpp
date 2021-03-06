#include "PimInternal.h"
#include <sstream>

#include "PimFont.h"
#include "PimLabel.h"
#include "PimGameControl.h"
#include "PimHelperFunctions.h"
#include "PimAssert.h"

namespace Pim {
	/*
	=====================
	Font::Font
	=====================
	*/
	Font::Font(string font, int psize, bool bilinearFiltering) {
		//font = Pim::GameControl::getModulePath().append(font);

		charWidth	= NULL;
		tex			= NULL;
		size		= psize;
		filter		= bilinearFiltering;
		Init(font, size);
	}

	/*
	=====================
	Font::~Font
	=====================
	*/
	Font::~Font(void) {
		Clean();
	}

	/*
	=====================
	Font::GetCharacterWidth
	=====================
	*/
	int Font::GetCharacterWidth(const char ch) const {
		if (charWidth) {
			return (int)charWidth[ch];
		}

#ifdef _DEBUG
		printf("WARNING: Font uninitalized!\n");
#endif /* _DEBUG */

		return 0;
	}

	/*
	=====================
	Font::Init
	=====================
	*/
	void Font::Init(string font, int size) {
		FT_Error error;
		FT_Library libFT = NULL;
		FT_Face face = NULL;

		error = FT_Init_FreeType(&libFT);
		if (error) {
			PimWarning("Could not initialize FreeType.", "FreeType error!");
			return;
		}

		error = FT_New_Face(libFT, font.c_str(), 0, &face);
		if (error == FT_Err_Unknown_File_Format) {
			FT_Done_FreeType(libFT);
			string errstr = "Could not recognize format of file:\n";
			errstr.append(font);
			
			PimWarning(errstr.c_str(), "FreeType error!");
			return;
		} else if (error) {
			FT_Done_FreeType(libFT);

			stringstream ss;
			ss << error;

			string errstr = "Unkown error opening file (";
			errstr.append( ss.str() );
			errstr.append("):\n");
			errstr.append(font);
			
			PimWarning(errstr.c_str(), "FreeType error!");
			return;
		}

		error = FT_Set_Char_Size(
					face,
					size << 6,
					size << 6,
					96,
					96 );
		if (error) {
			PimWarning("Unable to set the character size.", "Freetype error!");
			return;
		}

		RenderToTexture(face);

		FT_Done_Face(face);
		FT_Done_FreeType(libFT);
	}

	/*
	=====================
	Font::RenderToTexture
	=====================
	*/
	void Font::RenderToTexture(FT_Face face) {
		// Clean up if the textures already exist
		Clean();

		// Begin rendering the bitmaps to the textures
		tex = new GLuint[128];
		charWidth = new unsigned char[128];

		listBase = glGenLists(128);
		glGenTextures(128, tex);

		for (unsigned char i=0; i<128; i++) {
			CreateDisplayList(face, i);
		}
	}

	/*
	=====================
	Font::CreateDisplayList
	=====================
	*/
	void Font::CreateDisplayList(FT_Face face, char ch) {
		if (FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), 
			FT_LOAD_DEFAULT | FT_LOAD_MONOCHROME)) {
			PimWarning("Error: FT_Load_Glyph failed!", "FreeType error!");
			return;
		}

		FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		FT_Bitmap bitmap = face->glyph->bitmap;

		int width = NextPow2(bitmap.width);
		int height = NextPow2(bitmap.rows);

		GLubyte *expandedData = new GLubyte[2 * width * height];

		for (int j=0; j<height; j++) {
			for (int i=0; i<width; i++) {
				expandedData[2*(i+j*width)] = expandedData[2*(i+j*width)+1] =
												  (i>=bitmap.width || j>=bitmap.rows) ?
												  0 : bitmap.buffer[i + bitmap.width*j];
			}
		}

		glBindTexture(GL_TEXTURE_2D, tex[ch]);

		if (filter) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		} else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
					 GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expandedData);


		glNewList(listBase+ch, GL_COMPILE);		// Display list begin

		glBindTexture(GL_TEXTURE_2D, tex[ch]);
		glPushMatrix();

		glTranslatef(
			(float)face->glyph->bitmap_left,
			(float)face->glyph->bitmap_top - bitmap.rows,
			0.f
		);

		float x = float(bitmap.width) / float(width);
		float y = float(bitmap.rows) / float(height);

		glBegin(GL_QUADS);
			glTexCoord2f(0.f,0.f); glVertex2i(0, bitmap.rows);
			glTexCoord2f(0.f,  y); glVertex2i(0, 0);
			glTexCoord2f(  x,  y); glVertex2i(bitmap.width, 0);
			glTexCoord2f(  x,  0); glVertex2i(bitmap.width, bitmap.rows);
		glEnd();

		glPopMatrix();

		glTranslatef(float(face->glyph->advance.x >> 6), 0.f, 0.f);

		glEndList();							// Display list end
		
		charWidth[ch] = (unsigned char)(face->glyph->advance.x >> 6);

		delete[] expandedData;
	}

	/*
	=====================
	Font::Clean
	=====================
	*/
	void Font::Clean() {
		if (tex) {
			glDeleteLists(listBase, 128);
			glDeleteTextures(128, tex);
			delete[] tex;
			delete[] charWidth;
		}
	}
}