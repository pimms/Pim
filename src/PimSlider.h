#pragma once
#include "pimgamenode.h"
#include "PimButton.h"


namespace Pim {
	class Sprite;
	
	/**
	 @class 		SliderCallback
	 @brief 		Inheriting from this class allows you to receive Slider 
	 				callback values.
	 */
	class SliderCallback {
	public:
		virtual void			SliderValueChanged(Slider *slider, float val) {}
	};
	
	
	/**
	 @class 		Slider
	 @brief   		The Slider object is a UI-element class intended for creating 
	 				a slider capable of sliding in any direction.
	 
	 				Once instantiated, none of the values can be changed. The 
	 				exception to this is the minimum and maximum return value. 
	 			
					You @b cannot create a slider sliding in the X axis and
	 				later modify that object to slide in the Y axis. 
	 */
	class Slider : public Pim::GameNode, public Pim::ButtonCallback {
	public:
								Slider(Vec2 pointZero,			Vec2 pointMax,
									   Sprite *background,		Sprite *handleNormal,
									   Sprite *handleHovered,	Sprite *handlePressed,
									   Sprite *handleDeactivated);
		void					SetCallback(Pim::SliderCallback *cb);
		void					SetMinMaxValues(float min, float max);
		void					SetHandlePosition(float val);
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
	
	/**
	 @fn 			Slider::Slider
	 @brief			The longest constructor I've ever written. I'm sorry.
	 @param			pointZero
	 				Relative to the background's anchor. Is not axis bound. 
	 				This is the position which will yield the lowest value set.
	 @param  		pointMax
	 				Relative to the background's anchor. Is not axis bound. This 
	 				is the position which will yield the highest value.
	 @param 		background
	 				The background sprite of the slider.
	 @param 		handle@e[xxxx]
	 				Four different sprites illustrating the different states the slider can
	 				have. Only "handleNormal" is required. These four sprites are used to
	 				create a Pim::Button which will behave like a handle.
	 */

	/**
	 @fn 			SetMinMaxValues
	 @brief 		Set the minimum and maximum return values for the Slider.
	 @param 		min
	 				The value returned when the slider handle is at the pointZero
	 				position passed in the constructor.
	 @param 		max
	 				The value returned when the slider handle is at the pointMax
	 				position passed in the constructor.
	 */
	
	/**
	 @fn 			SetHandlePosition
	 @brief 		Set the handle at position @e val relative to the return range.
	 @details 		Example:
	 	
	 				If @e minPt = [0,0] and @e maxPt = [10,0] and the return 
					ranges from 0 to 1, a @e val value of 0.5 will put the slider
	 				at position [5,0].
	 */
}