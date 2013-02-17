#pragma once

#include "PimInternal.h"
#include "PimGameNode.h"

namespace Pim {
// Forward declarations
class GameControl;
class Label;

class Font {
	private:
		friend class Label;

	public:
							Font(string font, int size, bool bilinearFiltering = true);
							~Font(void);
		int					GetCharacterWidth(const char ch) const;

	private:
		unsigned char		*charWidth;	// Font width - different for some characters
		unsigned int		size;		// Font height - same for all characters
		GLuint				listBase;
		GLuint				*tex;		// The font texture
		bool				filter;		// BilinearFiltering?

							Font(const Font& other) {}
							Font()					{}
		void				Init(string fnt, int s);
		void				RenderToTexture(FT_Face face);
		void				CreateDisplayList(FT_Face face, char ch);
		void				Clean();
	};
}