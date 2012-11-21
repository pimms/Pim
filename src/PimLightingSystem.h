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

namespace Pim
{
	// Forward declarations
	class Layer;
	class GameNode;
	class Sprite;
	class Shader;
	struct Color;
	struct LightDef;

	// The LightSystem class is interacted with through the Layer owning the 
	// LightingSystem. Do not - unless you know what you're doing - do funny stuff.
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
		void preloadTexture(LightDef *lDef, std::string identifier);
		bool usePreloadedTexture(LightDef *lDef, std::string identifier);
		void deletePreloadedTexture(std::string identifier);

		// Render the lights onto a texture
		static void createSmoothLightTexture(LightDef *lDef, bool preload=false);
		static void createFlatLightTexture(LightDef *lDef, bool preload=false);

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

		// Preloaded light textures
		std::map<std::string, GLuint>		preloadTex;
	};
}