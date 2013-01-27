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

namespace Pim {
	class Sprite;

	class SliderCallback {
	public:
		virtual void					SliderValueChanged(Slider *slider, float val) {}
	};

	class Slider : public Pim::GameNode, public Pim::ButtonCallback {
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

								Slider(Vec2 pointZero,			Vec2 pointMax,
									   Sprite *background,		Sprite *handleNormal,
									   Sprite *handleHovered,	Sprite *handlePressed,
									   Sprite *handleDeactivated);
		void					SetCallback(Pim::SliderCallback *cb);
		void					SetMinMaxValues(float min, float max); // Set slider return values
		void					SetHandlePosition(float val); // Based on the minmax value
		void					SetActive(bool flag);
		float					GetValue() const;

	private:
		SliderCallback			*callback;
		Button					*handle;
		bool					dragging;
		float					minVal;
		float					maxVal;
		Vec2					axis;
		Vec2					minPt;
		Vec2					maxPt;
		const MouseEvent		*mouseEvent;

		void					OnMouseEvent(MouseEvent &evt);
		void					ButtonPressed(Button *button);
		void					ButtonReleased(Button *button);
	};
}