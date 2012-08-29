#pragma once

#include "pim.h"
#include <vector>

namespace Pim
{
	// Forward declarations
	class RenderWindow;
	class GameNode;
	class Layer;

	class GameControl
	{
	public:
		// Public access methods
		static GameControl* getSingleton()	
					{ return singleton; }
		static RenderWindow* getRenderWindow()		
					{ return singleton->mWindow; }
		static const std::string& getWindowTitle()	
					{ return singleton->mWindowTitle; }
		static int getWindowWidth()					
					{ return singleton->mWindowWidth; }
		static int getWindowHeight()				
					{ return singleton->mWindowHeight; }
		static Vec2 getWindowSize()				
					{ return Vec2((float)getWindowWidth(),(float)getWindowHeight()); }

		GameControl(std::string title, int w, int h);
		~GameControl();

		void go(Layer *l);

		void addInputListener(GameNode* n);
		void removeInputListener(GameNode* n);
		void addFrameListener(GameNode* n);
		void removeFrameListener(GameNode* n);

		// Deletes the old layer, replaces it with the passed.
		void setNewLayer(Layer *l);

	private:
		static GameControl* singleton;

		void gameLoop();
		void dispatchKeyInput();
		void dispatchMouseInput();
		void dispatchUpdate();

		RenderWindow	*mWindow;
		std::string		mWindowTitle;
		int				mWindowWidth;
		int				mWindowHeight;

		Layer			*layer;

		std::vector<GameNode*>		inputListeners;
		std::vector<GameNode*>		frameListeners;

		// Used to calculate delta time
		long long unsigned int ticks;
		float calculateDeltaTime();
	};

}