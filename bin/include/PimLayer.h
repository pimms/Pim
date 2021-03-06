#pragma once

#include "PimVec2.h"
#include "PimGameNode.h"
#include "PimLightingSystem.h"

namespace Pim {
	/**
	 @class 		Layer
	 @brief 		Defines a layer. Intermediate object most commonly used between
	 				the Scene and your Sprites.
	 @details 		The Layer derives from GameNode, and as such it can be added
	 				as child to any other GameNode — however, it is also @e required
	 				to rule your node hierarchy, only triumphed by your Scene.
	 
	 				A @e very basic node hierarchy has the following structure:
	 					Scene  -->  Layer  -->  Sprites
	 				
	 				However, given the dynamic nature of GameNodes, you may have
	 				something totally off the bat, like this:
	 					Scene  -->  Layer  --> GameNode  --> Layer  -->  Layer
	 			
	 				Note that in both the simple and complex example, the first
	 				two nodes are identical:  Scene  -->  Layer.
	 
	 				The Layer is capable of post processing shaders (performed by
	 				rendering the entire sub-tree of the Layer onto a texture and
	 				re-rendering the texture and applying the shader), and lighting
	 				system functionality using soft / hard shadows in real-time.
	 */
	
	
	class CollisionManager;
	class RenderTexture;
	class GameControl;
	class Scene;
	struct Color;
	struct PreloadLightDef;

	class Layer : public GameNode {
	protected:
		friend class Scene;
		friend class GameControl;

	public:

		Vec2					scale;
		bool					immovable;
		Color					color;
		Shader					*shader;

								Layer();
		virtual					~Layer();
		Scene*					GetParentScene() const;
		Layer*					GetParentLayer();	// Returns this
		Vec2					GetWorldPosition() const;
		Vec2					GetLayerPosition() const;
		float					GetWorldRotation() const;
		float					GetLayerRotation() const;
		void					SetImmovableLayer(bool immov);
		virtual void			Draw();
		virtual void			LoadResources() {}
		virtual void			SetZOrder(const int z);
		void					SetShader(Shader *shader);
		Color					GetColor() const;

		// ---------- LIGHTING SYSTEM METHODS ----------
		void					CreateLightingSystem(Vec2 resolution);
		void					DestroyLightingSystem();
		void					AddLight(GameNode *node, LightDef *lDef);
		bool					AddLight(GameNode *node, PreloadLightDef *pld, const string id);
		void					RemoveLight(GameNode *node);
		void					PreloadLightTexture(LightDef *ld, const string id);
		void					AddShadowCaster(GameNode *caster);
		void					RemoveShadowCaster(GameNode *caster);
		void					SetCastShadows(const bool shadows);
		void					SetLightingUnlitColor(const Color color);
		void					SetLightAlpha(const float a);
		void					SetSmoothShadows(const bool flag);
		void					SetShadowcasterDebugDraw(const bool);
		LightingSystem*			GetLightingSystem() const;

	protected:
		Scene					*parentScene;
		LightingSystem			*lightSys;

		void					PrepareRT();
		void					RenderRT() const;

	private:
		RenderTexture*			rt;			// Used for post processing effects (shader)
		Vec2					curRTRes;	// Resolution of the RT
	};
	
	/**
	 @fn 			SetImmovableLayer
	 @brief 		This layer will ignore it's parent's position.
	 */
	
	/**
	 @fn 			LoadResources
	 @brief 		Virtual method that @b MUST be used to load game-resources.
	 @details 		Sprite, Audio and Shader-loading often requires certain 
	 				singleton objects or internal member variables to be initialized,
	 				which will cause funny behaviour and crashes if anything is 
	 				attempted initialized in @e your Layer's constructor.
	 			
	 				Under no circumstances should you load sprites or audio 
	 				@e before LoadResources is called. Obviously, you are free
	 				to load whatever you want after this point, as the internal
	 				systems are guaranteed to be instantiated and ready.
	 */
	
	/**
	 @fn 			Layer::CreateLightingSystem
	 @brief 		Instantiate the LightingSystem in this Layer.
	 @details 		There is only one LightingSystem available to each layer.
	 @param 		resolution
	 				The resolution of the LightingSystem's render-texture.
	 			
					The resolution is independent from the render-resolution
	 				defined in CreationData, which enables you to use high res
	 				lights and shadows in a low-res retro-styled game.
	 */
	
	/**
	 @fn 			Layer::DestroyLightingSystem
	 @brief 		Destroys the currently active LightingSystem (if there is one).
	 */
	
	/**
	 @fn 			Layer::AddLight
	 @brief 		Makes a GameNode act as a light in the Layer's LightingSystem.
	 @param 		node
	 				A node which need not be a child of the Layer.
	 @param 		lDef
	 				A LightDef which defined how the light-texture is rendered.
	 				The LightingSystem takes ownership (handles cleanup) of lDef.
	 */
	
	/**
	 @fn 			Layer::AddLight
	 @brief 		Makes a GameNode act as a light in the Layer's LightingSystem.
	 				The light will use a pre-loaded light texture, as defined by
	 				@e pld and @e id. The preloaded LightDef @b must have been 
	 				added via @e PreloadLightTexture() prior to calling this.
	 @param 		pld
					The size of the light is the only value read from @e pld. 
					The LightingSystem takes ownership (handles cleanup) of pld.
	 @param 		id
					The unqiue preloaded light-def identification string supplied
	 				when you called @e PreloadLightTexture().
	 */
	
	/**
	 @fn 			Layer::RemoveLight
	 @brief 		Remove the light functionality of a GameNode. The object is 
	 				@e not deleted.
	 */
	
	/**
	 @fn 			Layer::PreloadLightTexture
	 @brief 		Preload a light texture for future use. Useful when using
	 				multiple lights using @e roughly the same texture.
	 @param 		ld
	 				A LightDef-object, created as when creating a light normally
	 				using @e AddLight(GameNode*, @e LightDef*).
	 @param 		id
	 				The string uniquely used to identify the created texture and
	 				LightDef.
	 */
	
	/**
	 @fn 			Layer::AddShadowCaster
	 @brief 		Add a shadow caster to the LightingSystem.
	 @details 		The GameNode supplied @b absolutely @b must have created a
	 				shadow shape prior to being added. This is done by calling
	 				@e SetShadowShape().
	 */
	
	/**
	 @fn 			Layer::RemoveShadowCaster
	 @brief 		Remove the shadow casting functionality of a GameNode. The 
	 				shadow shape of the GameNode is @e not deleted.
	 */
	
	/**
	 @fn			Layer::SetCastShadows
	 @brief 		Enable or disable shadow casting.
	 */
	
	/**
	 @fn 			Layer::SetLightingUnlitColor
	 @brief 		Set the default "ambient" lighting - the color drawn as filler
	 				where no lights are present.
	 */
	
	/**
	 @fn 			Layer::SetLightAlpha
	 @brief 		Range: 0-1. Sets the visibility and brightness of the lights.
	 */
	
	/**
	 @fn 			Layer::SetSmoothShadows
	 @brief 		Enable or disable gaussian blur on the LightingSystem texture.
	 */
	
	/**
	 @fn 			Layer::SetShadowCasterDebugDraw
	 @brief 		Debug draw shadow casters. This is NOT related to the shadow-shape
	 				debug drawing of the GameNode-class.
	 */
}