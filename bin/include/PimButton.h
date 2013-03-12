#pragma once
#include "PimGameNode.h"

namespace Pim {
	class Sprite;
	class Button;
	class Slider;

	
	/**
	 @class 		ButtonCallback
	 @brief 		May be inherited from by any class to receive Button-events.
	 */
	class ButtonCallback {
	public:
		virtual void			ButtonPressed(Button*)		{}
		virtual void			ButtonReleased(Button*)		{}
		virtual void			ButtonHoverBegin(Button*)	{}
		virtual void			ButtonHoverEnd(Button*)		{}
	};

	
	/**
	 @class 		Button
	 @brief 		GUI element providing button functionality.
	 */
	class Button : public Pim::GameNode {
	public:
		enum ButtonState {
			NORMAL,
			HOVERED,
			PRESSED,
			DEACTIVATED,
		};

		bool					activated; 
		bool					allowLeftClick;
		bool					allowRightClick;

								Button(Sprite* normal, Sprite* hovered=NULL, 
										Sprite* pressed=NULL, Sprite* deactivated=NULL);
		void					OnMouseEvent(MouseEvent&);
		void					ReplaceSprite(ButtonState state, Sprite *sprite);
		void					SetActiveState(ButtonState state);
		void					SetActive(bool flag);
		void					SetCallback(ButtonCallback *cb);

	protected:
		Sprite					*sprites[4];
		Sprite					*current;
		bool					mouseDownLastFrame;
		bool					mouseHoverLastFrame;
		bool					mouseFreshWhileHover;	// Was the current mouse click created while hovering?
		ButtonCallback*			callback;

		virtual bool			IsHovered(const Pim::Vec2 mousePos) const;
		virtual void			MakeNormalCurrent();
		virtual void			MakeHoveredCurrent();
		virtual void			MakePressedCurrent();
		virtual void			MakeDeactivatedCurrent();
	};
	
	/**
	 @fn 		Button::Button
	 @brief 	The @b only constructor for this class.
	 
	 @details	The class takes a wholly 4 Sprites to create. Note that only one
	 			is required. 
	 
	 @param 	normal
	 			The sprite containing the normal (unhovered, unpressed) texture
				for the button. @b Required!
	 @param 	hovered
	 			The sprite containing the hovered texture for this button. @e Optional.
	 @param 	pressed
	 			The sprite containing the hovered texture for this button. @e Optional.
	 @param 	deactivated
	 			The sprite containing the deactivated texture for this button. @e Optional.
	 */
	
	/**
	 @fn 		Button::ReplaceSprite
	 @brief 	Replace the sprite used for one of the button states.
	 @param 	state
	 			The concerned state.
	 @param 	sprite
	 			The new sprite.
	 */
	
	/**
	 @fn 		Button::SetActive
	 @brief 	Disable / enable the button.
	 @details 	When disabled, no callbacks will occur and the button will not 
	 			function. The @e deactivated sprite will be used for disabled
				buttons.
	 */
	
	/**
	 @fn 		Button::SetCallback
	 @brief 	Define a callback object for this button.
	 @param 	cb
	 			@b MUST be of type ButtonCallback!!!
	 
	 			This object will receive callbacks upon one of the four Button
	 			events occurance.
	 */
}