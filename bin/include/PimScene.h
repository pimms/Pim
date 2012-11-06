#pragma once

#include "PimInternal.h"
#include "PimVec2.h"

/*
	The Scene class does not derive from GameNode, as it's functionality is strictly
	limited to the handling of layers.
*/

namespace Pim
{
	// Forward declarations
	class RenderWindow;
	class GameControl;
	class Layer;

	class Scene
	{
	public:
		Scene();
		virtual ~Scene();

		void addLayer(Layer *layer);
		void removeLayer(Layer *layer);

		virtual void loadResources() {}

		bool	dirtyZOrder;

		Color	color;

	protected:
		friend class GameControl;
		friend class RenderWindow;

		void drawScene();		// Called from RenderWindow's renderFrame().
		void drawSceneHUD();	// Called after drawScene().

		void orderLayers();		// Duplicate of GameNode::orderChildren.

		std::vector<Layer*>		layers;
	};
}
