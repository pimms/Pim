#pragma once
#include "PimGameNode.h"

namespace Pim {
	/**
	 @class 		Label
	 @brief 		GUI element for displaying text on the screen.
	 @details 		A Font-object must be added to a label object upon creation.
	 				Failure to do so will cause an error. 
	 				
	 				The Label is by default @b not the 'owner' of the Font-object,
	 				and the Font must be @e deleted manually by you. However, you
	 				may call @e GiveOwnershipOfFont() on a Label - this will cause
	 				the Label to @e delete the Font upon it's own death.
	 
	 				@b IMPORTANT
	 				
	 				All values regarding the Label (bounding rectangle, line 
	 				padding, etc.) is in @e PIXEL @e SPACE, and works regardless
	 				of the coordinate-system defined in the active CreationData.
	 */
	
	class Font;

	class Label : public GameNode {
	public:
		enum TextAlignment {
			TEXT_LEFT,
			TEXT_CENTER,
			TEXT_RIGHT,
		};

		const Font						*font;
		Vec2							scale;
		Color							color;

										Label(const Font *pfont);
										Label(const Font *pfont, const string ptext);
										~Label();
		void							SetFont(const Font *pfont);
		virtual void					SetText(const string ptext);
		virtual void					SetTextWithFormat(const char *ptext, ...);
		void							SetTextAlignment(const TextAlignment align);
		void							SetLinePadding(const int pad); // Line distance
		void							CalculateDimensions();
		Vec2							GetDimensions() const;
		void							Draw();
		void							BatchDraw();
		void							GiveOwnershipOfFont();

	protected:
		Vec2							dim;
		vector<string>					lines;
		vector<int>						lineWidth;

	private:
		unsigned int					linePadding;
		Vec2							anchor;
		bool							fontOwner;
	};
	
	/**
	 @fn 			SetText
	 @brief 		Define the text to display.
	 */
	
	/**
	 @fn 			SetTextWithFormat
	 @brief 		Define the text with a format. The format specifiers are the 
	 				same as when using printf. @b THE @b STRING @b MUST @b BE @b \\n
	 				@b TERMINATED!!!
	 */
	
	/**
	 @fn 			SetTextAlignment
	 @brief 		Define the alignment of the text.
	 @details 		The placement of the lines is placed as the TextAlignment
					values desribe; left aligned, center aligned or right aligned.
	 @param 		align
					Each line in the string is placed inside the rectangle (as per
	 				defined by the widest lines) containing the label.
	 */
	
	/**
	 @fn 			SetLinePadding
	 @brief 		Set the padding between a multi-lined Label (in screen pixels).
	 */
	
	/**
	 @fn 			CalculateDimensions
	 @brief 		Calculate the dimensions of the label. Called internally.
	 */
	
	/**
	 @fn 			GiveOwnershipOfFont
	 @brief 		The Label is given charge of it's Font.
	 @details 		The Label will clean up the Font when it's deallocated.
					
	 				@b NOTE: This method shold not be called unless the respective
					Font is used only by one Label, or you have absolute control 
	 				over @e all your Labels using this Font. Use with caution!
	 */
}
