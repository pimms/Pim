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
		// PARAMETERS:
		// font		- The relative path to your font. Example: "resources/arial.ttf"
		// size		- The height of the font in pixels.
		// bilinearFiltering	- Filtering of the font textures. Adviced to be turned on
		//						  for most apps, but retro low-res games benefit
		//						  from disabling filtering.
		Font(std::string font, int size, bool bilinearFiltering = true);
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

		bool			filter;		// BilinearFiltering?

		// Forbid use of these initializers
		Font(const Font& other) {}
		Font() {}		
	};
}