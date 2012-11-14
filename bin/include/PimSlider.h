#pragma once
#include "pimgamenode.h"
#include "PimButton.h"

/*
	The Slider object is a UI-element class intended for creating a slider
	capable of sliding in any direction.

	Once instantiated, none of the values can be changed. The exception to this
	is the minimum and maximum return value. For instance, you cannot first create
	a slider sliding in the X axis, and later modify that object to slide in the
	Y axis.
*/

namespace Pim
{
	// Forward declarations
	class Sprite;
	
	// The SliderCallback class. Inherit from this if you want callbacks.
	class SliderCallback
	{
	public:
		virtual void sliderValueChanged(Slider *slider, float val) {}
	};

	// The Slider class.
	class Slider : public Pim::GameNode, public Pim::ButtonCallback
	{
	public:
		/*
			The slider constructor takes in total seven parameters,
			3 of which are optional.

			pointZero:
				Relative to the background's anchor. Is not axis bound. This is the position 
				which will yield the lowest value set.
			pointMax:
				Relative to the background's anchor. Is not axis bound. This is the position
				which will yield the highest value.
			background:
				The background sprite of the slider.
			handleXXXX:
				Four different sprites illustrating the different states the slider can
				have. Only "handleNormal" is required. These four sprites are used to
				create a Pim::Button which will behave like a handle.
		*/
		Slider(
			Vec2 pointZero,			Vec2 pointMax,
			Sprite *background,		Sprite *handleNormal, 
			Sprite *handleHovered,	Sprite *handlePressed, 
			Sprite *handleDeactivated);
		
		// Set the callback
		void setCallback(Pim::SliderCallback *cb);

		// Set the lowest and the highest return values for the slider.
		// Defaults to {0.0 - 1.0}.
		void setMinMaxValues(float min, float max);

		// Sets the slider position to max the value given (in the range provided to
		// be the minimum and lowest value, defaults to {0.0 - 1.0}). This initiates
		// a callback.
		void setHandlePosition(float val);

		void setActive(bool flag);
		
		// Returns the current value
		float getValue();

	private:
		// ButtonCallback and Mouse callbacks
		void mouseEvent(MouseEvent &evt);
		void buttonPressed(Button *button);
		void buttonReleased(Button *button);

		SliderCallback	*callback;

		Button		*handle;
		bool		dragging;

		float		minVal, maxVal;

		// The normalized vector of the min-point subtracted from the max-point. 
		Vec2		axis;
		Vec2		minPt, maxPt;

		MouseEvent	*mEvt;
	};
}