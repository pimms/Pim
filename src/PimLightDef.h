#pragma once

#include "PimInternal.h"
#include "PimVec2.h"

namespace Pim
{
	// For the sake of the LightDef hierarchy, these definitions are required.
	// The superclass, "LightDef" needs to be abstract, in order to separate
	// wether the LightDef is "Smooth" or "Flat".
	#define ABSTRACT_SUPERCLASS private: virtual void ____abstract____() = 0; public:
	#define NONABSTRACT_SUBCLASS private: void ____abstract____(){} public:

	// Forward declarations
	class LightingSystem;
	struct Color;

	// Superclass for definement of lights
	struct LightDef
	{
		ABSTRACT_SUPERCLASS 

		LightDef();
		~LightDef();

		GLuint getTexID();
		
		Color	innerColor;		// The inner color - Def=(1,1,1,1)
		Color	outerColor;		// The outer color, the alpha SHOULD be 0 - Def=(0.2,0.2,0.2,0.0)
		float	radius;			// The radius, and width&height of the light texture. Def=128
		bool	castShadows;	// Should this light cast shadows?	Def=true

		float	falloff;		// The falloff rate

		Vec2	position;		// Relative to the parent's position

	protected:
		friend class LightingSystem;

		short	lightType;		// 0=flat, 1=smooth
		GLuint	lTex;			// Texture containing the light itself
		bool	isPreloaded;	// Is the texture used preloaded?
	};

	// Flat lights have the same color in the radius area, and a "falloff" zone
	// in a percentage of the radius, defined in "falloff" (0.0 - 1.0).
	struct FlatLightDef : public LightDef
	{
		NONABSTRACT_SUBCLASS
		FlatLightDef();
	};

	// More  customizeable than the flat lighting def
	struct SmoothLightDef : public LightDef
	{
		NONABSTRACT_SUBCLASS
		SmoothLightDef();

		// How many times the center should be rendered - the inner circle
		// is the area with the radius (radius*falloff).
		int innerPasses;
	};

	// Does nothing but indicate that the lightdef uses a preloaded texture.
	// Only the radius value requires a value, all other member variables are ignored.
	struct PreloadLightDef : public LightDef
	{
		NONABSTRACT_SUBCLASS
		PreloadLightDef();
	};
}