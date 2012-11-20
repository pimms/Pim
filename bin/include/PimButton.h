#pragma once
#include "PimGameNode.h"

namespace Pim
{
	// Forward declarations
	class Sprite;
	class Button;
	class Slider;

	// The callback class
	class ButtonCallback
	{
	public:
		virtual void buttonPressed(Button*)		{}
		virtual void buttonReleased(Button*)	{}
		virtual void buttonHoverBegin(Button*)	{}
		virtual void buttonHoverEnd(Button*)	{}
	};


	// The button class
	class Button : public Pim::GameNode
	{
	public:
		enum ButtonState 
		{
			NORMAL,
			HOVERED,
			PRESSED,
			DEACTIVATED,
		};

		Button(Sprite* normal, Sprite* hovered=NULL, Sprite* 
			pressed=NULL, Sprite* deactivated=NULL);

		void replaceSprite(ButtonState state, Sprite*);

		void setActiveState(ButtonState state);

		void mouseEvent(MouseEvent&);

		void setActive(bool flag);

		void setCallback(ButtonCallback *cb);

		// Is the button currently activated?
		bool activated;

		// Left click will be accepted as a click if true. Defaults to true.
		bool allowLeftClick;
		// Right click will be accepted as a click if true. Defaults to false.
		bool allowRightClick;

	protected:

		virtual void makeNormalCurrent();
		virtual void makeHoveredCurrent();
		virtual void makePressedCurrent();
		virtual void makeDeactivatedCurrent();

		virtual bool isHovered(Pim::Vec2 mousePos);

		// The sprites for the different states.
		// 0: Normal (required)
		// 1: Hovered
		// 2: Pressed
		// 3: Deactivated
		Sprite *sprites[4];
		
		// The currently active sprite
		Sprite *current;

		// Was the mouse pressed last frame?
		bool mouseDownLastFrame;

		// Was the mouse hovering last frame?
		bool mouseHoverLastFrame;

		// Was the current mouse click created while hovering? 
		bool mouseFreshWhileHover;

		// The object we're messaging
		ButtonCallback *callback;
	};
}