#include "PimSlider.h"
#include "PimSprite.h"
#include "PimButton.h"
#include "PimAssert.h"
#include "PimInput.h"
#include "PimLayer.h"

namespace Pim {
	/*
	=====================
	Slider::Slider
	=====================
	*/
	Slider::Slider(	Vec2 pointZero,			Vec2 pointMax,
					Sprite *background,		Sprite *handleNormal,
					Sprite *handleHovered,	Sprite *handlePressed,
					Sprite *handleDeactivated) {
		handle = new Pim::Button(
			handleNormal, handleHovered,
			handlePressed, handleDeactivated
		);

		AddChild(background);
		AddChild(handle);
		handle->SetCallback(this);
		handle->position = Vec2(20,0);

		// Figure out the axis and relative distance
		minPt = pointZero;
		maxPt = pointMax;
		axis = (pointMax-pointZero).Normalize();

		handle->position = maxPt;

		callback	= NULL;
		dragging	= false;
		minVal		= 0.f;
		maxVal		= 1.f;

		ListenMouse();
	}


	/*
	=====================
	Slider::SetCallback
	=====================
	*/
	void Slider::SetCallback(SliderCallback *cb) {
		callback = cb;
	}

	/*
	=====================
	Slider::SetMinMaxValues
	=====================
	*/
	void Slider::SetMinMaxValues(float min, float max) {
		PimAssert(min < max, "Error: value 'min' must be lower than 'max'!");
		minVal = min;
		maxVal = max;
	}

	/*
	=====================
	Slider::SetHandlePosition
	=====================
	*/
	void Slider::SetHandlePosition(float val) {
		float fac = (val-minVal) / (maxVal-minVal);
		if (fac < 0.f) {
			fac = 0.f;
		} else if (fac > 1.f) {
			fac = 1.f;
		}

		handle->position = minPt + (maxPt-minPt)*fac;

		if (callback) {
			callback->SliderValueChanged(this,GetValue());
		}
	}

	/*
	=====================
	Slider::SetActive
	=====================
	*/
	void Slider::SetActive(bool flag) {
		handle->SetActive(flag);
	}

	/*
	=====================
	Slider::GetValue
	=====================
	*/
	float Slider::GetValue() const {
		float fac = (handle->position-minPt).Length() / (maxPt-minPt).Length();
		return fac*(maxVal-minVal) + minVal;
	}

	/*
	=====================
	Slider::OnMouseEvent
	=====================
	*/
	void Slider::OnMouseEvent(MouseEvent &evt) {
		mouseEvent = &evt;

		if (dragging) {
			if (!evt.IsKeyDown(MouseEvent::MBTN_LEFT)) {
				dragging = false;
				return;
			}

			handle->SetActiveState(Button::PRESSED);

			Vec2 offset = GetParentLayer()->position + GetLayerPosition() + minPt;
			Vec2 handlePos = minPt + axis * axis.Dot(evt.GetPosition() - offset);

			// Have we moved too far towards the minPt?
			if (axis.Dot(handlePos-minPt) < 0.f) {
				handlePos = minPt;
			} else if (handlePos.Length() > maxPt.Length()) {
				handlePos = maxPt;
			}

			handle->position = handlePos;

			if (callback) {
				callback->SliderValueChanged(this,GetValue());
			}
		}
	}

	/*
	=====================
	Slider::ButtonPressed
	=====================
	*/
	void Slider::ButtonPressed(Button *btn) {
		if (btn == handle) {
			dragging = true;
		}
	}

	/*
	=====================
	Slider::ButtonReleased
	=====================
	*/
	void Slider::ButtonReleased(Button *btn) {
		if (btn == handle) {
			dragging = false;
		}
	}
}