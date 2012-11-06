#pragma once

#include "PimInternal.h"
#include "PimVec2.h"
#include "PimWinStyle.h"

namespace Pim
{
	// Forward declarations
	class RenderWindow;
	class GameNode;
	class Layer;
	class Scene;

	class GameControl
	{
	public:
		// Public access methods
		static GameControl* getSingleton()	
					{ return singleton; }
		static RenderWindow* getRenderWindow()
					{ return singleton->renderWindow; }
		static const std::string& getWindowTitle()	
					{ return singleton->winData.winTitle; }
		static int getWindowWidth()					
					{ return singleton->actualWinWidth; }
		static int getWindowHeight()				
					{ return singleton->actualWinHeight; }
		static Vec2 getWindowSize()				
					{ return Vec2((float)getWindowWidth(),(float)getWindowHeight()); }
		static const WinStyle::CreationData getCreationData()
					{ return singleton->winData; }
		static Scene* getScene()
					{ return singleton->scene; }
		static std::string getModulePath();	

		GameControl();
		~GameControl();

		// Parameters: 
		// Scene *s:
		//		Pointer to a scene object. Cannot be nil. Deleted automatically.
		// CreationData:
		//		The structure describing how the window should behave.
		// commandline:
		//		Flag indicating whether or not the console should create a new thread
		//		which reads input from the command line. 
		void go(Scene *s, WinStyle::CreationData data, bool commandline=true);

		void addKeyListener(GameNode* n);
		void removeKeyListener(GameNode* n);

		void addMouseListener(GameNode* n);
		void removeMouseListener(GameNode* n);

		void addFrameListener(GameNode* n);
		void removeFrameListener(GameNode* n);

		// Change the window methods. Never ever deal with the render window
		// by yourself. That's a stupid thang to do.
		void setWindowCreationData(WinStyle::CreationData data);
		void setWindowStyle(WinStyle::WinStyle style);
		void setWindowResolution(int w, int h);

		// Returns the position of GLPoint (0,0) - adjusted by the black borders
		Vec2 lowerLeftCorner();

		// As the resolution shifts, the app is scaled accordingly. This is that scale.
		Vec2 windowScale();

		// In case of forced coordinates, this is the factor a position in pixel coordinates
		// must be multiplied by in order to achieve the expected result.
		// The method's main application is converting mouse screen coordinates to fit the 
		// forced coordinate system.
		// Returns [1,1] if forcedCoordinateSystem is false.
		Vec2 coordinateFactor();

		// Deletes the old layer, replaces it with the passed.
		void setScene(Scene *newScene);


		

	protected:
		friend class RenderWindow;

		static GameControl* singleton;

		void gameLoop();
		void dispatchPrerender();
		void dispatchPostrender();

		WinStyle::CreationData	winData;
		int						actualWinWidth;		// The values in winData does NOT apply if
		int						actualWinHeight;	// the window style is BFS. Hence, these two.

		RenderWindow			*renderWindow;
		Scene					*scene;

		std::vector<GameNode*>	frameListeners;

		std::string				modulePath;

		// Used to calculate delta time
		long long unsigned int ticks;
		float calculateDeltaTime();
	};

}