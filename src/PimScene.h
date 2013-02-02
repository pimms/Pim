#pragma once

#include "PimInternal.h"
#include "PimVec2.h"
#include "PimGameControl.h"

/*
	The Scene class does not derive from GameNode, as it's functionality is strictly
	limited to the handling of layers.
*/

namespace Pim {
	class Layer;

	class Scene {
	protected:
		friend class GameControl;
		friend class RenderWindowBase;
#ifdef WIN32
		friend class RenderWindowWIN;
#endif

	public:
		bool					dirtyZOrder;
		Color					color;

								Scene();
		virtual					~Scene();
		virtual void			Update(float dt){}	
		virtual void			LoadResources() {}	// Load your layers here
		virtual Layer*			PauseLayer();		// Return your pause layer here
		void					AddLayer(Layer *layer);
		void					RemoveLayer(Layer *layer);

	protected:
		vector<Layer*>			layers;

		void					DrawScene();
		void					OrderLayers();
	};
}
