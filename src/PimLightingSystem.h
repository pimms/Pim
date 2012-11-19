#pragma once

#include "PimInternal.h"
#include "PimRenderTexture.h"

/*
	The lighting system is the boss handling shadow casting, rendering of light textures
	and in general just being a boss. A boss, boss, boss.

	In order to add shadows (or light) to your layer, call "createLightingSystem()" on it.
	Then toggle light receptiveness on the children of said layer.

	Note that a lighting system attached to layer A is 100% independent of a lighting 
	system attached to layer B. However, children of any layers attached to A or B will
	still be affected by A or B's lighting system. In other words, light systems affect
	_everything_ attached to their layer - children, grandchildren, the children of a 
	children layer etc.

	The best practice is also to, at any given time, only have one lighting system enabled.

	The layer using the lightingsystem can also NOT be the child of a batch node.

	---

	The lighting system is quite simple. The shadow map is drawn completely black, then
	the lights (game nodes) are rendered atop this black surface. The shadows are simply
	_not_ rendered light; when a collision between a light and a shadow caster has occured,
	the light is simply ignoring to draw in this area, thus creating an _illusion_ of shadow.

	Hopefully rendering light textures rather than a light circle will prove to be faster
	and look better.

	---

	The light textures are rendered so that the inner color segment is innerFallof * radius
	pixels in length. This is the brightest area, and it defaults to a low value (0.2).
	The innter area's alpha is innerColor.a in the center, and 0 at the edge.
	The rest of the light is then rendered with (oc) in the center, and (oc+(0,0,0,0.2)) at
	the outer edge, where "oc" is the outerColor variable.
*/

// For the sake of the LightDef hierarchy, these definitions are required.
// The superclass, "LightDef" needs to be abstract, in order to separate
// wether the LightDef is "Smooth" or "Flat".
#define ABSTRACT_SUPERCLASS virtual void ____abstract____() = 0;
#define NONABSTRACT_SUBCLASS void ____abstract____(){}

namespace Pim
{
	// Forward declarations
	class Layer;
	class GameNode;
	class LightingSystem;
	class Sprite;
	class Shader;
	struct Color;

	// Superclass for definement of lights
	struct LightDef
	{
		ABSTRACT_SUPERCLASS

		LightDef()
		{
			innerColor		= Color(1.f, 1.f, 1.f, 1.f);
			outerColor		= Color(0.2f, 0.2f, 0.2f, 0.0f);
			castShadows		= true;
			radius			= 128;
			lTex			= 0;
			falloff			= 0.2f;
			lightType		= -1;
		}
		~LightDef()
		{
			if (lTex)
			{
				glDeleteTextures(1, &lTex);
			}
		}

		GLuint getTexID()
		{
			return lTex;
		}

		
		Color	innerColor;		// The inner color - Def=(1,1,1,1)
		Color	outerColor;		// The outer color, the alpha SHOULD be 0 - Def=(0.2,0.2,0.2,0.0)
		int		radius;			// The radius, and width&height of the light texture. Def=128
		bool	castShadows;	// Should this light cast shadows?	Def=true

		float	falloff;

	protected:
		friend class LightingSystem;

		short	lightType;		// 0=flat, 1=smooth

		GLuint lTex;			// Texture containing the light itself
	};

	// Flat lights have the same color in the radius area, and a "falloff" zone
	// in a percentage of the radius, defined in "falloff" (0.0 - 1.0).
	struct FlatLightDef : public LightDef
	{
		NONABSTRACT_SUBCLASS

		FlatLightDef()
		{
			lightType = 0;
		}
	};

	// More  customizeable than the flat lighting def
	struct SmoothLightDef : public LightDef
	{
		NONABSTRACT_SUBCLASS

		SmoothLightDef()
		{
			lightType = 1;
			innerPasses = 3;
		}

		// How many times the center should be rendered - the inner circle
		// is the area with the radius (radius*falloff).
		int innerPasses;
	};

	class LightingSystem
	{
	protected:
		// Only layers can instantiate lighting systems. Call layer->createLightingSystem().
		friend class Layer;

		LightingSystem(Layer*, Vec2 resolution);
		LightingSystem(const LightingSystem &o) {}
		~LightingSystem();
		
		void loadShaders();	

		// Called by layers - updates the uniform in the shader
		void setUnlitColor(Color c);
		void setLightAlpha(float a);

		// Adds a light, and renders a light texture based on the data in light def
		void addLight(GameNode *node, LightDef *lDef);

		// Render the lights onto a texture
		static void createSmoothLightTexture(LightDef *lDef);
		static void createFlatLightTexture(LightDef *lDef);

		virtual void renderLightTexture();		// The main rendering called every frame
		void gaussPass();

		virtual void renderLights();
		virtual void renderShadows(LightDef *d, GameNode *n, Vec2 &p, Vec2 &rResSc);

		Layer									*parent;

		std::map<GameNode*,LightDef*>			lights;
		std::vector<Sprite*>					casters;

		bool									hqShadow;

		bool									castShadow;

		bool									dbgDrawNormal;	// Casting edges and normals

		Vec2									resolution;		// Shadowtex resolution

		Color									color;			// Color of the unlit areas

		RenderTexture	*mainRT;
		RenderTexture	*gaussRT;

		// The shader used when rendering the light texture onto the main buffer
		Shader			*shaderLightTex;

		// Gaussian blur shader used to illude smooth shadows
		Shader			*shaderGauss;
	};
}