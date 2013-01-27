#pragma once

#include "PimInternal.h"
#include "PimVec2.h"
#include "PimWinStyle.h"

namespace Pim {
	class RenderWindow;
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
		void					DispatchPrerender(float dt);
		void					DispatchPausedPreRender(float dt);
		void					DispatchPreRender_r(GameNode *n, float dt);
		float					CalculateDeltaTime();
		void					ClearDeleteQueue();
		void					SceneTransition();
	};
}