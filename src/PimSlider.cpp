#include "PimSlider.h"
#include "PimSprite.h"
#include "PimButton.h"
#include "PimException.h"
#include "PimInput.h"
#include "PimLayer.h"

namespace Pim
{
	Slider::Slider(	Vec2 pointZero,			Vec2 pointMax,
					Sprite *background,		Sprite *handleNormal, 
					Sprite *handleHovered,	Sprite *handlePressed, 
					Sprite *handleDeactivated)
	{
		handle = new Pim::Button(
			handleNormal, handleHovered,
			handlePressed, handleDeactivated
			);

		addChild(background);
		addChild(handle);
		handle->setCallback(this);
		handle->position = Vec2(20,0);

		// Figure out the axis and relative distance
		minPt = pointZero;
		maxPt = pointMax;
		axis = (pointMax-pointZero).normalize();

		handle->position = maxPt;

		callback	= NULL;
		dragging	= false;
		minVal		= 0.f;
		maxVal		= 1.f;

		listenMouse();
	}

	void Slider::setCallback(SliderCallback *cb)
	{
		callback = cb;
	}
	void Slider::setMinMaxValues(float min, float max)
	{
		PimAssert(min < max, "Error: value 'min' must be lower than 'max'!");
		minVal = min;
		maxVal = max;
	}
	void Slider::setHandlePosition(float val)
	{
		float fac = (val-minVal) / (maxVal-minVal);
		if (fac < 0.f)
			fac = 0.f;
		else if (fac > 1.f)
			fac = 1.f;

		handle->position = minPt + (maxPt-minPt)*fac;

		if (callback)
			callback->sliderValueChanged(this,getValue());
	}
	float Slider::getValue()
	{
		float fac = (handle->position-minPt).length() / (maxPt-minPt).length();
		return fac*(maxVal-minVal) + minVal;
	}

	void Slider::mouseEvent(MouseEvent &evt)
	{
		mEvt = &evt;

		if (dragging)
		{
			if (!evt.isKeyDown(MouseEvent::MBTN_LEFT))
			{
				dragging = false;
				return;
			}

			handle->setActiveState(Button::PRESSED);

			Vec2 offset = getParentLayer()->position + getLayerPosition() + minPt;
			Vec2 hPos = minPt + axis * axis.dot(evt.getPosition() - offset);

			// Have we moved too far towards the minPt?
			if (axis.dot(hPos-minPt) < 0.f)
				hPos = minPt;
			else if (hPos.length() > maxPt.length())
				hPos = maxPt;

			handle->position = hPos;

			if (callback)
				callback->sliderValueChanged(this,getValue());
		}
	}
	void Slider::buttonPressed(Button *btn)
	{
		if (btn == handle)
			dragging = true;
	}
	void Slider::buttonReleased(Button *btn)
	{
		if (btn == handle)
			dragging = false;
	}
}