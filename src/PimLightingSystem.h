#pragma once

#include "Stdafx.h"

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
	class LightingSystem;
	class Sprite;
	class Shader;
	struct Color;

	// The light def is holding values defining how the light should be rendered.
	struct LightDef
	{
		LightDef();
		~LightDef()
		{
			if (lTex)
				glDeleteTextures(1, &lTex);
		}
		
		Color	innerColor;		// The inner color - Def=(1,1,1,1)
		Color	outerColor;		// The outer color, the alpha SHOULD be 0 - Def=(0.2,0.2,0.2,0.0)
		int		radius;			// The radius, and width&height of the light texture. Def=128
		bool	castShadow;		// Should this light cast shadows?	Def=true

		float innerFalloff;		// How fast the inner (brightest) area should fall. (0.0-1.0)
								// Default is 0.2. 

	private:
		friend class LightingSystem;

		GLuint lTex;
	};

	namespace ShadowTechnique
	{
		typedef enum ShadowTechnique
		{
			HARD,		// UNIMPLEMENTED
			BLUR,		// UNIMPLEMENTED
			SOFT,		// UNIMPLEMENTED
		};
	}

	class LightingSystem
	{
	public:

	private:
		// Only layers can instantiate lighting systems. Call layer->createLightingSystem().
		friend class Layer;

		LightingSystem(Layer*);
		LightingSystem(const LightingSystem &o) {}
		~LightingSystem();

		void loadShaders();

		// Adds a light, and renders a light texture based on the data in light def
		void addLight(GameNode *node, LightDef *lDef);

		// Render the light texture.
		void createLightTexture(LightDef *lDef);

		void renderLightTexture();		// The main rendering called every frame
		void _renderLights();			// Merely a subroutine

		Layer									*parent;
		ShadowTechnique::ShadowTechnique		tech;

		std::map<GameNode*,LightDef*>			lights;
		std::vector<Sprite*>					casters;

		bool									castShadow;
		bool									useMultShader;

		// OpenGL
		GLuint			frameBuffer;
		GLuint			texID;
		Shader			*shader;

		// The framebuffer and texture containing the shadows
		GLuint			shadowFBO;
		GLuint			shadowTex;
	};
}