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

		// The update calls are made automatically on the currently active
		// scene object. 
		virtual void update(float dt) {}

		virtual void loadResources() {}

		// Called from the GameController when GameControl::pause() is called. If this
		// method returns NULL, the game is not paused.
		virtual Layer* pauseLayer();

		bool	dirtyZOrder;

		Color	color;

	protected:
		friend class GameControl;
		friend class RenderWindow;

		void drawScene();		// Called from RenderWindow's renderFrame().

		void orderLayers();		// Duplicate of GameNode::orderChildren.

		std::vector<Layer*>		layers;
	};
}
