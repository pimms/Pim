#include "PimButton.h"
#include "PimSprite.h"
#include "PimInput.h"
#include "PimGameControl.h"
#include "PimRenderWindow.h"
#include "PimLayer.h"

// Macro used to check whether one or both of the keys are down.
// It multiplies the boolean value of the available buttons with 
// the return value of whether the buttons are pressed or not.
#define MOUSE_DOWN	\
	(	(evt.isKeyDown(MouseEvent::MBTN_LEFT) && allowLeftClick)	||	\
		(evt.isKeyDown(MouseEvent::MBTN_RIGHT) && allowRightClick)		)
#define MOUSE_FRESH \
	(	(evt.isKeyFresh(MouseEvent::MBTN_LEFT) && allowLeftClick)	||	\
		(evt.isKeyFresh(MouseEvent::MBTN_RIGHT) && allowRightClick)		)

namespace Pim
{
	Button::Button(Sprite* normal, Sprite* hovered, Sprite* pressed, Sprite* deactivated)
	{
		listenMouse();

		activated			= true;
		allowLeftClick		= true;
		allowRightClick		= false;
		callback			= NULL;
		mouseDownLastFrame	= false;
		mouseHoverLastFrame	= false;
		mouseFreshWhileHover = false;

		for (int i=0; i<4; i++) sprites[i]=NULL;

		current = normal;
		sprites[NORMAL] = normal;
		addChild(sprites[NORMAL]);

		if (hovered)
		{
			sprites[HOVERED] = hovered;
			sprites[HOVERED]->hidden = true;
			addChild(sprites[HOVERED]);
		}

		if (pressed)
		{
			sprites[PRESSED] = pressed;
			sprites[PRESSED]->hidden = true;
			addChild(sprites[PRESSED]);
		}

		if (deactivated)
		{
			sprites[DEACTIVATED] = deactivated;
			sprites[DEACTIVATED]->hidden = true;
			addChild(sprites[DEACTIVATED]);
		}
	}

	void Button::mouseEvent(MouseEvent &evt)
	{
		if (activated) 
		{
			if (isHovered(evt.getPosition()))
			{
				if (!mouseHoverLastFrame)
				{
					// The mouse was hovered over
					if (mouseDownLastFrame)
						mouseDownLastFrame = MOUSE_DOWN;
					if (!mouseDownLastFrame)
						mouseFreshWhileHover = false;

					makeHoveredCurrent();
					if (callback)
						callback->buttonHoverBegin(this); 
				}

				if (mouseFreshWhileHover && mouseDownLastFrame && !MOUSE_DOWN)
				{
					mouseDownLastFrame = false;
					mouseFreshWhileHover = false;

					// The mouse was released over the button
					makeHoveredCurrent();

					if (callback)
						callback->buttonReleased(this);
				}
				
				if (MOUSE_FRESH || (mouseFreshWhileHover && !mouseHoverLastFrame))
				{
					// The mouse was pressed over the button
					mouseDownLastFrame = true;
					mouseFreshWhileHover = true;
					makePressedCurrent();
					if (callback)
						callback->buttonPressed(this);
				}

				mouseHoverLastFrame = true;
			}
			else if (mouseHoverLastFrame)
			{
				// The button is no longer hovered
				if (mouseDownLastFrame)
					mouseDownLastFrame = MOUSE_DOWN;
				if (!mouseDownLastFrame)
					mouseFreshWhileHover = false;

				mouseHoverLastFrame = false;

				makeNormalCurrent();
				if (callback)
					callback->buttonHoverEnd(this);
			}
			else
			{
				if (mouseDownLastFrame)
					mouseDownLastFrame = MOUSE_DOWN;
				if (!mouseDownLastFrame)
					mouseFreshWhileHover = false;

				if (current != sprites[NORMAL])
					makeNormalCurrent();
			}
		}
	}

	bool Button::isHovered(Pim::Vec2 mousePos)
	{
		Vec2 cFac = GameControl::getSingleton()->coordinateFactor();
		Vec2 wFac = GameControl::getSingleton()->windowScale();

		Rect temp = current->rect;
		temp.width *= cFac.x * wFac.x * abs(current->scale.x);
		temp.height *= cFac.y * wFac.y * abs(current->scale.y);
		
		Vec2 p = current->getLayerPosition();
		temp.x = p.x - (temp.width * current->anchor.x);
		temp.y = p.y - (temp.height * current->anchor.y);

		return temp.contains(mousePos - current->getParentLayer()->position);
	}

	void Button::setActive(bool flag)
	{
		activated = flag;

		if (!activated)
			makeDeactivatedCurrent();
		else
			makeNormalCurrent();
	}

	void Button::setCallback(ButtonCallback *cb)
	{
		callback = cb;
	}

	void Button::replaceSprite(ButtonState state, Sprite *spr)
	{
		if (sprites[state])
			removeChild(sprites[state], true);

		sprites[state] = spr;
		sprites[state]->hidden = true;
		addChild(sprites[state]);
	}

	void Button::setActiveState(ButtonState state)
	{
		if (state == NORMAL) 
			makeNormalCurrent();
		else if (state == HOVERED)
			makeHoveredCurrent();
		else if (state == PRESSED)
			makePressedCurrent();
		else 
			makeDeactivatedCurrent();
	}
	void Button::makeNormalCurrent()
	{
		if (sprites[NORMAL])
		{
			current->hidden = true;
			current = sprites[NORMAL];
			current->hidden = false;
		}
	}
	void Button::makeHoveredCurrent()
	{
		if (sprites[HOVERED])
		{
			current->hidden = true;
			current = sprites[HOVERED];
			current->hidden = false;
		}
		else
			makeNormalCurrent();
	}
	void Button::makePressedCurrent()
	{
		if (sprites[PRESSED])
		{
			current->hidden = true;
			current = sprites[PRESSED];
			current->hidden = false;
		}
	}
	void Button::makeDeactivatedCurrent()
	{
		if (sprites[DEACTIVATED])
		{
			current->hidden = true;
			current = sprites[DEACTIVATED];
			current->hidden = false;
		}
	}
}