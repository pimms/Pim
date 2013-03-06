#pragma once

#include "PimInternal.h"
#include "PimVec2.h"

namespace Pim {
	/**
	 @class 		LightDef
	 @brief 		Defines parameters for how LightingSystem renders a light
	 				texture.
	
	 @var 			innerColor
	 				The innermost color of the light.
	 @var 			outerColor
	 				The outermost color of the light.
	 @var 			radius
	 				The radius of the light (in LightingSystem-resolution pixel space).
	 @var 			falloff
	 				Linear falloff factor for how quickly the innerColor transitions
	 				to outerColor. <NOT YET FULLY IMPLEMENTED>
	 */
	
	#define ABSTRACT_SUPERCLASS private: virtual void ____abstract____() = 0; public:
	#define NONABSTRACT_SUBCLASS private: void ____abstract____(){} public:

	class LightingSystem;
	struct Color;

	struct LightDef {
	protected:
		friend class LightingSystem;

		ABSTRACT_SUPERCLASS

		Color			innerColor;	
		Color			outerColor;	
		float			radius;			// The radius, and width&height of the light texture. Def=128
		bool			castShadows;	// Should this light cast shadows?	Def=true
		float			falloff;		// The falloff rate
		Vec2			position;		// Relative to the parent's position

						LightDef();
						~LightDef();
		GLuint			GetTexID() const;

	protected:
		short			lightType;		// 0=flat, 1=smooth
		GLuint			lTex;			// Texture containing the light itself
		bool			isPreloaded;	// Is the texture used preloaded?
	};

	// Flat lights have the same color in the radius area, and a "falloff" zone
	// in a percentage of the radius, defined in "falloff" (0.0 - 1.0).
	struct FlatLightDef : public LightDef {
		NONABSTRACT_SUBCLASS
						FlatLightDef();
	};

	// More  customizeable than the flat lighting def
	struct SmoothLightDef : public LightDef {
		NONABSTRACT_SUBCLASS

		int				innerPasses;

						SmoothLightDef();
	};

	// Does nothing but indicate that the lightdef uses a preloaded texture.
	// Only the radius value requires a value, all other member variables are ignored.
	struct PreloadLightDef : public LightDef {
		NONABSTRACT_SUBCLASS
						PreloadLightDef();
	};
}