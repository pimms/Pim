#include "PimButton.h"
#include "PimSprite.h"
#include "PimInput.h"
#include "PimGameControl.h"
#include "PimRenderWindow.h"
#include "PimLayer.h"

#define MOUSE_DOWN	\
	(	(evt.IsKeyDown(MouseEvent::MBTN_LEFT) && allowLeftClick)	||	\
		(evt.IsKeyDown(MouseEvent::MBTN_RIGHT) && allowRightClick)		)
#define MOUSE_FRESH \
	(	(evt.IsKeyFresh(MouseEvent::MBTN_LEFT) && allowLeftClick)	||	\
		(evt.IsKeyFresh(MouseEvent::MBTN_RIGHT) && allowRightClick)		)

namespace Pim {

	/*
	=====================
	Button::Button
	=====================
	*/
	Button::Button(Sprite* normal, Sprite* hovered, Sprite* pressed, Sprite* deactivated) {
		ListenMouse();

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
		AddChild(sprites[NORMAL]);

		if (hovered) {
			sprites[HOVERED] = hovered;
			sprites[HOVERED]->hidden = true;
			AddChild(sprites[HOVERED]);
		}

		if (pressed) {
			sprites[PRESSED] = pressed;
			sprites[PRESSED]->hidden = true;
			AddChild(sprites[PRESSED]);
		}

		if (deactivated) {
			sprites[DEACTIVATED] = deactivated;
			sprites[DEACTIVATED]->hidden = true;
			AddChild(sprites[DEACTIVATED]);
		}
	}

	/*
	=====================
	Button::MouseEvent
	=====================
	*/
	void Button::OnMouseEvent(MouseEvent &evt) {
		if (activated) {
			if (IsHovered(evt.GetPosition())) {
				if (!mouseHoverLastFrame) {
					// The mouse was hovered over
					if (mouseDownLastFrame) {
						mouseDownLastFrame = MOUSE_DOWN;
					} else {
						mouseFreshWhileHover = false;
					}

					MakeHoveredCurrent();
					if (callback)
						callback->ButtonHoverBegin(this);
				}

				if (mouseFreshWhileHover && mouseDownLastFrame && !MOUSE_DOWN) {
					mouseDownLastFrame = false;
					mouseFreshWhileHover = false;

					// The mouse was released over the button
					MakeHoveredCurrent();

					if (callback) {
						callback->ButtonReleased(this);
					}
				}

				if (MOUSE_FRESH || (mouseFreshWhileHover && !mouseHoverLastFrame)) {
					// The mouse was pressed over the button
					mouseDownLastFrame = true;
					mouseFreshWhileHover = true;
					MakePressedCurrent();
					if (callback)
						callback->ButtonPressed(this);
				}

				mouseHoverLastFrame = true;
			} else if (mouseHoverLastFrame) {
				// The button is no longer hovered
				if (mouseDownLastFrame) {
					mouseDownLastFrame = MOUSE_DOWN;
				} else {
					mouseFreshWhileHover = false;
				}

				mouseHoverLastFrame = false;

				MakeNormalCurrent();
				if (callback) {
					callback->ButtonHoverEnd(this);
				}
			} else {
				if (mouseDownLastFrame) {
					mouseDownLastFrame = MOUSE_DOWN;
				} else {
					mouseFreshWhileHover = false;
				}

				if (current != sprites[NORMAL]) {
					MakeNormalCurrent();
				}
			}
		}
	}

	/*
	=====================
	Button::ReplaceSprite
	=====================
	*/
	void Button::ReplaceSprite(ButtonState state, Sprite *spr) {
		if (sprites[state]) {
			RemoveChild(sprites[state], true);
		}

		sprites[state] = spr;
		sprites[state]->hidden = true;
		AddChild(sprites[state]);
	}

	/*
	=====================
	Button::SetActiveState
	=====================
	*/
	void Button::SetActiveState(ButtonState state) {
		if (state == NORMAL) {
			MakeNormalCurrent();
		} else if (state == HOVERED) {
			MakeHoveredCurrent();
		} else if (state == PRESSED) {
			MakePressedCurrent();
		} else {
			MakeDeactivatedCurrent();
		}
	}

	/*
	=====================
	Button::SetActive
	=====================
	*/
	void Button::SetActive(bool flag) {
		activated = flag;

		if (!activated) {
			MakeDeactivatedCurrent();
		} else {
			MakeNormalCurrent();
		}
	}

	/*
	=====================
	Button::SetCallback
	=====================
	*/
	void Button::SetCallback(ButtonCallback *cb) {
		callback = cb;
	}

	/*
	=====================
	Button::IsHovered
	=====================
	*/
	bool Button::IsHovered(const Pim::Vec2 mousePos) const {
		Vec2 cFac = GameControl::GetSingleton()->GetCoordinateFactor();
		Vec2 wFac = GameControl::GetSingleton()->GetWindowScale();

		Rect temp = current->rect;
		temp.width *= cFac.x * wFac.x * abs(current->scale.x);
		temp.height *= cFac.y * wFac.y * abs(current->scale.y);

		Vec2 p = current->GetLayerPosition();
		temp.x = p.x - (temp.width * current->anchor.x);
		temp.y = p.y - (temp.height * current->anchor.y);

		return temp.Contains(mousePos - current->GetParentLayer()->position);
	}

	/*
	=====================
	Button::MakeNormalCurrent
	=====================
	*/
	void Button::MakeNormalCurrent() {
		if (sprites[NORMAL]) {
			current->hidden = true;
			current = sprites[NORMAL];
			current->hidden = false;
		}
	}

	/*
	=====================
	Button::MakeHoveredCurrent
	=====================
	*/
	void Button::MakeHoveredCurrent() {
		if (sprites[HOVERED]) {
			current->hidden = true;
			current = sprites[HOVERED];
			current->hidden = false;
		} else {
			MakeNormalCurrent();
		}
	}

	/*
	=====================
	Button::MakePressedCurrent
	=====================
	*/
	void Button::MakePressedCurrent() {
		if (sprites[PRESSED]) {
			current->hidden = true;
			current = sprites[PRESSED];
			current->hidden = false;
		}
	}

	/*
	=====================
	Button::MakeDeactivatedCurrent
	=====================
	*/
	void Button::MakeDeactivatedCurrent() {
		if (sprites[DEACTIVATED]) {
			current->hidden = true;
			current = sprites[DEACTIVATED];
			current->hidden = false;
		}
	}
}