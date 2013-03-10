#pragma once

#include "PimInternal.h"
#include "PimVec2.h"
#include "PimWinStyle.h"
#include "PimRenderWindow.h"

namespace Pim {
	/**
	 @class 	GameControl
	 
	 @brief  	The foundational class of Pim.
	 
	 @details 	GameControl handles input via SDL, is in charge of executing
	 			the game-loop and is in general a friendly boss.
	 			
	 			The GameControl singleton should be created within the main()
	 			function of your app, and @e must be passed a WinStyle::CreationData
	 			object.
	 
	 			Direct interaction with the GameControl singleton is sometimes 
	 			needed, but mostly for administrative purposes like limiting
	 			the framerate or pausing the game.
	 */
	
	
	class GameNode;
	class Layer;
	class Scene;

	class GameControl {
	private:
		friend class RenderWindow;

	public:
								GameControl();
								~GameControl();
		static string			GetModulePath();
		static GameControl*		GetSingleton();
		static RenderWindow*	GetRenderWindow();
		static const string&	GetWindowTitle();
		static int				GetWindowWidth();
		static int				GetWindowHeight();
		static Vec2				GetWindowSize();
		static Scene*			GetScene();
		WinStyle::CreationData	GetCreationData() const;
		void					Go(Scene *s, WinStyle::CreationData data, bool commandline=true);
		void					AddKeyListener(GameNode* n);
		void					RemoveKeyListener(GameNode* n);
		void					AddMouseListener(GameNode* n);
		void					RemoveMouseListener(GameNode* n);
		void					AddControlListener(GameNode *n);
		void					RemoveControlListener(GameNode *n);
		void					AddFrameListener(GameNode* n);
		void					RemoveFrameListener(GameNode* n);
		void					Exit();
		void					LimitFrame(unsigned int maxfps);	// Pass 0 to set unlimited
		void					Pause();	// You must return a pause layer from your Scene
		void					Unpause();
		void					SetWindowCreationData(WinStyle::CreationData data);
		void					SetWindowStyle(WinStyle::WinStyle style);
		void					SetWindowResolution(int w, int h);
		Vec2					LowerLeftCorner();
		Vec2					GetWindowScale();
		Vec2					GetCoordinateFactor();
		void					SetScene(Scene *newScene);
		void					AddNodeToDelete(GameNode *node);

	private:
		static GameControl		*singleton;
		RenderWindow			*renderWindow;
		Scene					*scene;
		Scene					*newScene;
		vector<GameNode*>		frameListeners;
		vector<GameNode*>		delQueue;
		string					modulePath;
		bool					quit;
		bool					paused;
		Layer					*pauseLayer;
		float					maxDelta;
		bool					sleepNextFrame;
		float					sleepTime;
		long long unsigned int	ticks;
		WinStyle::CreationData	winData;
		int						actualWinWidth;		// The values in winData does NOT apply if
		int						actualWinHeight;	// the window style is BFS. Hence, these two.

		void					GameLoop();
		void					HandleEvents();
		void					DispatchPrerender(float dt);
		void					DispatchPausedPreRender(float dt);
		void					DispatchPreRender_r(GameNode *n, float dt);
		float					CalculateDeltaTime();
		void					ClearDeleteQueue();
		void					SceneTransition();
		void					ReloadTextures();
	};
	
	/**
	 @fn 		string GameControl::GetModulePath()
	 @brief 	Returns the working directory of the app @b (Windows @b only)
	 */
	
	/**
	 @fn 		GameControl* GameControl::GetSingleton()
	 @brief 	Returns the currently active singleton object
	 */
	
	/**
	 @fn		GameControl::Go
	 @brief 	Start the game and initiate the game-loop. The method will @e not
	 			return until the application is terminated.
	 
	 @param s	
	 			A subclass of Pim::Scene. 
	 @param data
	 			The CreationData object defining the appearance of the window.
	 @param commandline 
	 			@b WINDOWS @b ONLY. Whether or not to create a thread dedicated
	 			to reading input from the command line interface. This is @b only
	 			available in debug builds. See Pim::CommandListener for details.
	 */
	/**
	 @fn  		GameControl::LimitFrame
	 @brief 	Limit the framerate to @e maxfps.
	 
	 @param maxfps	
				The maximum allowed number of frames per second.
	 */
	
	/**
	 @fn 		GameControl::SetScene
	 @brief 	Transition to another scene.
	 @details 	At the end of the next game-loop iteration, the current scene
	 			will be replaced by @e newScene. The current scene is
				deleted and cleaned up after.
	 
	 			Example usage:
	 
	 @code 		
			Pim::Scene *myScene = new Pim::Scene();
			Pim::GameControl::GetSingleton()->SetScene(myScene);
	 @endcode
	 
	 @param newScene
			 	A new and instantiated Pim::Scene object.
	 */
	
	/**
	 @fn 		GameControl::LowerLeftCorner
	 @brief 	Returns the bottom left coordinate of the currently active Scene,
	 			excluding the borders created when the aspect ratio is forced.
	 
	 @details 	When the aspect ratio is forced, black borders are added either 
	 			vertically or horizontally. 
	 
	 @return 	If the borders are vertical and are 50px high, the return value 
	 			of this function will be [0, 50].
	 
	 			If the borders are horizontal and are 30px wide, the return value
				of this function will be [30, 0].
	 */
	
	/**
	 @fn		GameControl::GetWindowScale
	 @brief 	The relation between the size of the window in pixels (without 
	 			borders) and the render-resolution as specified in the CreationData
	 			object.
	 */
	
	/**
	 @fn 		GameControl::GetCoordinateFactor
	 @brief 	The relation between the size of the window in pixels (without 
	 			borders) and the coordinates specified in the CreationData object.
	 */
	
	/**
	 @fn 		GameControl::SetWindowResolution
	 @brief 	Change the resolution.
	 */
	
	/**
	 @fn 		GameControl::Pause
	 @brief 	Pause the game.
	 
	 @details 	Upon calling this method, the currently active Scene object
	 			is queried for a Layer to place atop the currently active scene.
	 			You can provide GameControl with a pause-layer by overriding the
	 			"virtual Layer* PauseLayer()" method in your Scene-classes. The 
	 			unoverriden method returns NULL, and if NULL is returned, the 
	 			game is @b NOT paused.
	 
	 			All children of the provided pause-layer receives all possible
	 			update calls as long as the game is paused.
	
	 			The pause layer is deleted automatically upon resuming the game.
				
	 			When pausing the game, the currently active scene is paused,
				and does not receive input updates or frame updates. 
	 
	 			Example:
	 @code
	 			// Somewhere in your code:
	 			GameControl::GetSingleton()->Pause();
	 
	 			// In your Scene class:
	 			Layer* MyScene::PauseLayer() {
	 				return new MyCustomPauseLayer();
	 			}
	 @endcode
	 */
	
	/**
	 @fn 		GameControl::AddNodeToDelete
	 @brief 	Deletes an object at a safe point in the future.
	 
	 @param 	node
	 			The object to be deleted.
	 
	 @details 	If you did now know this, any GameNode with a parent (i.e., it is
	 			in use by the application) should @b NOT, and I repeat @b NOTATALL
	 			be deleted via @e delete. This is stupidly important, and ignoring
	 			this warning @e will lead to crashes almost immidiately. 
	 
				Delete nodes by calling @e RemoveChild(...) on their parent. This 
	 			will add that node to the @e delete-queue, and the object will be
	 			deleted at a safe point in the future. After an object has been
	 			detached from it's parent safely, Pim will handle the deletion -
	 			but to you the object can be treated as deallocated.
	 */
	
	/**
	 @fn 		GameControl::ClearDeleteQueue
	 @brief 	Deletes all object currently in the delete queue.
	 
	 @details	Objects notify the GameControl object whenever one of their children
	 			has been scheduled for deletion via @e RemoveChild(). In order to
	 			prevent crashes from Actions and other scheduled events, the objects
	 			must be deleted when no outer loop or object is depending on that
	 			specific object to be alive.
				
	 			This method is called at several points during the game-loop, so
	 			delete-queue objects are rarely alive for more than a few milliseconds
	 			after they have been removed from their parent.
	 */
	
	
	/**
	 @fn 		void GameControl::Exit()
	 @brief		Quit the game after the @e next game-loop iteration
	 */
}