#pragma once

#include "Stdafx.h"
#include "PimGameNode.h"

namespace Pim
{
	// Forward declarations
	class GameControl;
	class Label;
	
	class Font
	{
	public:
		Font(std::string font, int size);
		~Font(void);

		int getCharacterWidth(const char ch);

	private:
		friend class Label;

		// Init method
		// Called from the various constructor methods
		void init(std::string fnt, int s);		

		// Render the bitmap font to textures
		void renderToTexture(FT_Face face);
		void createDisplayList(FT_Face face, char ch);

		// Cleanup method
		void clean();

		unsigned char	*charWidth;	// Font width - different for some characters
		unsigned int	size;		// Font height - same for all characters

		GLuint			listBase;	
		GLuint			*tex;		// The font texture

		// Forbid use of these initializers
		Font(const Font& other) {}
		Font() {}		
	};
}