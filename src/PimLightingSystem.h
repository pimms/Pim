#pragma once

#include "PimInternal.h"
#include "PimRenderTexture.h"

namespace Pim {
	/**
	 @class 		LightingSystem
	 @brief 		Provides support for lights and shadows in a Layer.
	 @details 		The lighting system is the boss handling shadow casting, 
	 				rendering of light textures and in general just being a boss. 
	 				A boss, boss, boss.
	 
				 	In order to add shadows (or light) to your layer, call 
	 				@e CreateLightingSystem() on it. Then toggle light 
	 				receptiveness on the children of said layer.
	 
	 				Note that a lighting system attached to layer A is 100% 
	 				independent of a lighting system attached to layer B. However, 
	 				children of any layers attached to A or B will still be affected 
	 				by A or B's lighting system. In other words, light systems affect
	 				_everything_ attached to their layer - children, grandchildren, 
	 				the children of a children layer etc.
	 
	 				The layer using the lightingsystem can also NOT be the child 
	 				of a batch node.
	 
	 				The lighting system is quite simple. The shadow tex is drawn 
	 				completely black, then the lights (game nodes) are rendered 
	 				atop this black surface. The shadows are simply @b not rendered 
	 				light; when a collision between a light and a shadow caster has occured,
	 				the light is simply ignoring to draw in this area, thus creating an 
	 				@e illusion of shadow.
	 
	 				The light textures are rendered so that the inner color segment 
	 				is innerFallof * radius pixels in length. This is the brightest 
	 				area, and it defaults to a low value (0.2). The innter area's 
	 				alpha is innerColor.a in the center, and 0 at the edge. The 
	 				rest of the light is then rendered with (oc) in the center, 
	 				and (oc+(0,0,0,0.2)) at the outer edge, where "oc" is the 
	 				outerColor variable.

					@b Normalmaps

					Each lighting system may contain several normal-maps. However,
					only 10 lights can @e affect the normal map may be used at 
					any given time in a particular lighting system.

					After adding a light to the lighting system, call 
					'SetNormalLighting(GameNode *light, bool flag)' to enable or
					disable normal map affection.
	 */

	
	class Layer;
	class GameNode;
	class Sprite;
	class Shader;
	struct Color;
	struct LightDef;

	class LightingSystem {
	protected:
		friend class Layer;
	
	public:

		static void						CreateSmoothLightTexture(LightDef *lDef, bool preload=false);
		static void						CreateFlatLightTexture(LightDef *lDef, bool preload=false);

										LightingSystem(Layer*, Vec2 resolution);
										~LightingSystem();

		Shader*							GetNormalMapShader();

		void							SetUnlitColor(const Color c);
		void							SetLightAlpha(float a);
		void							SetCastShadows(bool flag);
		void							SetDebugDrawShadowShapes(bool flag);

		void							AddLight(GameNode *node, LightDef *lDef);
		void							AddShadowCaster(GameNode *caster);
		void							RemoveLight(GameNode *light);
		void							RemoveShadowCaster(GameNode *caster);
		bool							SetNormalLighting(GameNode *light, bool flag);

		void							PreloadTexture(LightDef *lDef, const string identifier);
		bool							UsePreloadedTexture(LightDef *lDef, const string identifier);
		void							DeletePreloadedTexture(const string identifier);

		void							LoadShaders();
		virtual void					UpdateShaderUniforms();
		virtual void					RenderLightTexture();
		void							GaussPass();
		virtual void					RenderLights();
		virtual void					RenderShadows(LightDef *d,  GameNode *n, 
														const Vec2 &p, const Vec2 &rResSc);

	protected:

		static int						numSystemsCreated;

		int								number;			// This system's number
		Layer							*parent;
		map<GameNode*,LightDef*>		lights;			// Normal lights 
		vector<GameNode*>				normalLights;	// Lights affecting normal-maps
		vector<GameNode*>				casters;
		bool							hqShadow;
		bool							castShadow;
		bool							dbgDrawNormal;	// Casting edges and normals
		Vec2							resolution;		// Shadowtex resolution
		Color							color;			// Color of the unlit areas
		RenderTexture					*mainRT;
		RenderTexture					*gaussRT;
		Shader							*shaderLightTex;
		Shader							*shaderGauss;
		Shader							*shaderNormalMap;
		map<string, GLuint>				preloadTex;
	};
}