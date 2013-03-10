#pragma once

#include "PimInternal.h"
#include "PimGameNode.h"

namespace Pim {

	/**
	 @class 	Font
	 @brief 	Holds information on a font of a specific size.
	 @details 	The Font object is instantiated with a size and a font.
	 			A Label-object (or any other class) may use this object
	 			to render text to the screen.
	 
	 			@b IMPORTANT @b NOTE:
				
	 			Any Font object @b must be deleted manually, @b unless a Label
	 			has been given ownership of the Font - in which case, the Font
	 			is deleted along with the Label.
	 
	 			Example usage:
	 @code
				Pim::Font *arial20 = new Pim::Font("arial.ttf", 20);
	 			Pim::Label *helloWorld = new Pim::Label(arial20);
	 			helloWorld->SetText("Hello, world!");
	 			helloWorld->GiveOwnershipOfFont();
	 @endcode
	 */
	
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