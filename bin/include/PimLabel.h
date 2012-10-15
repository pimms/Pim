#pragma once
#include "PimGameNode.h"


namespace Pim
{
	// Forward declarations
	class Font;

	namespace Text
	{
		typedef enum TextAlignment
		{
			TEXT_LEFT,
			TEXT_CENTER,
			TEXT_RIGHT,
		};
	}

	class Label : public GameNode
	{
	public:
		// IMPORTANT: Must pass a non-nil value as Font *pfont!
		Label(Font *pfont);
		// IMPORTANT: Must pass a non-nil value as Font *pfont!
		Label(Font *pfont, std::string ptext);
		// IMPORTANT: Must pass a non-nil value as Font *pfont!
		void setFont(Font *pfont);

		// Set the text of the label. Add multiple lines by adding newline characters.
		void setText(std::string ptext);

		// Use this method to quickly add text with a variable format ("%0.1f", "%i").
		// Works just like you'd expect it to. Look up "printf()" for example usage.
		// Add multiple lines by adding newline characters.
		void setTextWithFormat(const char *ptext, ...);

		// Set the text alignment of the label
		void setTextAlignment(Text::TextAlignment align);

		// Set the line padding (distance between lines, render pixels)
		void setLinePadding(int pad);

		// Calculate the total width and height of the label.
		void calculateDimensions();

		// Gets the current dimensions with scale taken into account.
		// "return dimension_vector * scale_vector;"
		Vec2 getDimensions();

		// Render the string normally
		void draw();

		// Label's should not be children of SpriteBatchNodes. Inherited solely
		// to tell you if you're doing it wrong.
		void batchDraw();

		// WARNING: Calling this method WILL crash your app if the font used
		// by this label is used anywhere else. The font will be deleted
		// with this label. You will not have to manage the font yourself,
		// but be ridiculously careful.
		// The font must be set BEFORE calling this method.
		void giveOwnershipOfFont();


		// Font's are quite heavy to create, so you should only have a few font objects
		// alive in your app at any given time. If you need the same font with 
		// different sizes, create a font object with the highest font size, and scale
		// the labels smaller instead of creating multiple font objects.
		Font							*font;

		Vec2							scale;
		Color							color;

	private:
		unsigned int					linePadding;	

		Vec2							anchor;
		Vec2							dim;

		std::vector<std::string>		lines;
		std::vector<int>				lineWidth;

		bool							fontOwner;
	};
}
