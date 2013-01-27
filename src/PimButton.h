#pragma once
#include "PimGameNode.h"

namespace Pim {
	class Sprite;
	class Button;
	class Slider;


	class ButtonCallback {
	public:
		virtual void			ButtonPressed(Button*)		{}
		virtual void			ButtonReleased(Button*)		{}
		virtual void			ButtonHoverBegin(Button*)	{}
		virtual void			ButtonHoverEnd(Button*)		{}
	};


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
		void					ReplaceSprite(ButtonState state, Sprite*);
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
}