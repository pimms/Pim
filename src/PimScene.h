#pragma once

#include "PimInternal.h"
#include "PimVec2.h"
#include "PimGameControl.h"

namespace Pim {
	/**
	 @class 		Scene
	 @brief 		The topmost user class in the node hierarchy. @b Not a derivative
	 				of GameNode!
	@details 		At any given point, there can only be one active Scene. The
	 				scene is the highest level node in the node-tree, and all active
	 				nodes should be related to it.
	 
	 				You cannot add a GameNode to a Scene. The only object you can
	 				add to the Scene is a Layer, so the enforced structure is:
	 					Scene  -->  Layer  -->  <Whatever you please>
	 */
	
	class Layer;

	class Scene {
	protected:
		friend class GameControl;
		friend class RenderWindow;
		friend class RenderWindowWIN;

	public:
		bool					dirtyZOrder;
		Color					color;

								Scene();
		virtual					~Scene();
		virtual void			Update(float dt){}	
		virtual void			LoadResources() {}
		virtual Layer*			PauseLayer();
		int 					ChildCount();
		void					AddLayer(Layer *layer);
		void					RemoveLayer(Layer *layer);
		virtual void			ReloadTextures();

	protected:
		vector<Layer*>			layers;

		void					DrawScene();
		void					OrderLayers();
	};
	
	
	/**
	 @fn 			LoadResources
	 @brief 		The point in which you should load your Layers and other
	 				required resources / objects for your Scene.
	 @details 		Much like, and even more so, than the Layer class, the Scene
	 				derivatives @e must use LoadResources as the first possible
	 				point at which they may instantiate and load stuff.
	 
	 				After LoadResources has been called on your Scene, you are 
	 				free to load stuff at any point you'd like.
	 */
	
	/**
	 @fn 			PauseLayer
	 @brief 		Queried by GameControl when GameControl::Pause() is called.
	 @details 		In your Scene derivative, you must override this method if 
					you want to allow pausing (via Pim). Simply instantiate a 
	 				Layer of whatever description, and return it. If NULL is 
	 				returned (as is by default), the game will not pause.
	 */
	
	/**
	 @fn 			ReloadTextures
	 @brief 		Reloads all textures on nodes downward in the node-tree.
	 @details 		When the window is resized on OSX, the SDL-OpenGL context
	 				is destroyed, and all textures and shaders along with it.
	 			
	 				As a result, everything we hold dear must be reloaded.
	 
	 				This is not the case on Windows (as of 0.9).
	 */
}
