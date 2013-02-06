#include "PimInternal.h"
#include "PimGameControl.h"

#include "PimAssert.h"
#include "PimRenderWindow.h"
#include "PimInput.h"
#include "PimGameNode.h"
#include "PimLayer.h"
#include "PimShaderManager.h"
#include "PimAudioManager.h"
#include "PimScene.h"
#include "PimConsoleReader.h"

#include <climits>
#include <iostream>
#include <ctime>

namespace Pim {
	bool hasFocus		= false;	// The window has focus
	bool winHasMoved	= false;	// The window has been moved or resized

	/*
	=====================
	WndProc

	Window callback function
	=====================
	*/
	LRESULT	CALLBACK WndProc(HWND windowHandle, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch (msg) {
		case WM_ACTIVATE:
			if (wParam) {
				printf("gained focus\n");
				hasFocus = true;
				if (Input::GetSingleton())
					Input::GetSingleton()->_GainedFocus();
			} else {
				printf("lost focus\n");
				hasFocus = false;
				if (Input::GetSingleton())
					Input::GetSingleton()->_LostFocus();
			}
			return 0;


		case WM_SYSCOMMAND:
			if (wParam == 0xf012) {
				winHasMoved = true;
			}
			return DefWindowProc(windowHandle,msg,wParam,lParam);

		case WM_ENTERSIZEMOVE:
		case WM_MOVE:
		case WM_MOVING:
			winHasMoved = true;
			return 0;

		case WM_EXITSIZEMOVE:
			//winHasMoved = false;
			return 0;

		case WM_CLOSE:
			GameControl::GetSingleton()->Exit();
			return 0;

		case WM_KEYDOWN:
			//cout<<"Key: " <<wParam <<"\n";
			Input::GetSingleton()->_KeyPressed(wParam);
			return 0;

		case WM_KEYUP:
			Input::GetSingleton()->_KeyReleased(wParam);
			return 0;

			// MOUSE MOVE
		case WM_MOUSEMOVE:
			//cout<<"Move: " <<LOWORD(lParam) <<", " <<HIWORD(lParam) <<"\n";
			Input::GetSingleton()->_MouseMoved(LOWORD(lParam),HIWORD(lParam));
			return 0;

			// LEFT MOUSE
		case WM_LBUTTONDOWN:
			Input::GetSingleton()->_MousePressed(0);
			return 0;
		case WM_LBUTTONUP:
			Input::GetSingleton()->_MouseReleased(0);
			return 0;

			// RIGHT MOUSE
		case WM_RBUTTONDOWN:
			Input::GetSingleton()->_MousePressed(1);
			return 0;
		case WM_RBUTTONUP:
			Input::GetSingleton()->_MouseReleased(1);
			return 0;

		case WM_SIZE:
			GameControl::GetSingleton()->SetWindowResolution(LOWORD(lParam),HIWORD(lParam));
			return 0;
		}

		return DefWindowProc(windowHandle,msg,wParam,lParam);
	}



	GameControl* GameControl::singleton = NULL;


	/*
	=====================
	GameControl::GameControl
	=====================
	*/
	GameControl::GameControl() {
		PimAssert(singleton == NULL, "Only one GameControl instance can exist at a time.");

		singleton		= this;
		scene			= NULL;
		newScene		= NULL;

		paused			= false;
		pauseLayer		= NULL;

		// Limit initially to 200 fps
		maxDelta		= 1.f / 200.f;
		sleepNextFrame	= false;
		sleepTime		= 0.f;

		// Get the module path
		char path[260] = { '\0' };
		GetModuleFileName(NULL, path, 260);

		int lastSlash = 0;
		for (int i=0; i<260; i++) {
			if (path[i] == '\\' || path[i] == '/') {
				lastSlash = i;
			}
		}

		for (int i=lastSlash+1; i<260; i++) {
			path[i] = '\0';
		}

		modulePath = path;
		printf("Working directory:\n%s\n\n", modulePath.c_str());
	}

	/*
	=====================
	GameControl::~GameControl
	=====================
	*/
	GameControl::~GameControl() {
		singleton = NULL;

		if (renderWindow) {
			delete renderWindow;
		}
	}

	/*
	=====================
	GameControl::GetModulePath
	=====================
	*/
	string GameControl::GetModulePath() {
		return singleton->modulePath;
	}

	/*
	=====================
	GameControl::GetSingleton
	=====================
	*/
	GameControl* GameControl::GetSingleton() {
		return singleton;
	}

	/*
	=====================
	GameControl::GetRenderWindow
	=====================
	*/
	RenderWindow* GameControl::GetRenderWindow() {
		return singleton->renderWindow;
	}

	/*
	=====================
	GameControl::GetWindowTitle
	=====================
	*/
	const string& GameControl::GetWindowTitle() {
		return singleton->winData.winTitle;
	}

	/*
	=====================
	GameControl::GetWindowWidth
	=====================
	*/
	int GameControl::GetWindowWidth() {
		return singleton->actualWinWidth;
	}

	/*
	=====================
	GameControl::GetWindowHeight
	=====================
	*/
	int GameControl::GetWindowHeight() {
		return singleton->actualWinHeight;
	}

	/*
	=====================
	GameControl::GetWindowSize
	=====================
	*/
	Vec2 GameControl::GetWindowSize() {
		return Vec2(float(GetWindowWidth()), float(GetWindowHeight()));
	}

	/*
	=====================
	GameControl::GetScene
	=====================
	*/
	Scene* GameControl::GetScene() {
		return singleton->scene;
	}

	/*
	=====================
	GameControl::GetCreationData
	=====================
	*/
	WinStyle::CreationData GameControl::GetCreationData() const {
		return winData;
	}

	/*
	=====================
	GameControl::Go
	=====================
	*/
	void GameControl::Go(Scene *s, WinStyle::CreationData data, bool commandline) {
		try {
			winData = data;
			winData.Prepare();

			renderWindow = new RenderWindow(data);
			renderWindow->SetupWindow(data);

			#ifdef _DEBUG
			if (commandline) {
				ConsoleReader::Begin();
			}
			printf("\n[PIM-version %s]\n", PIM_VERSION);
			printf("[OpenGL-version %s]\n\n", glGetString(GL_VERSION));
			#endif /* _DEBUG */

			Input::InstantiateSingleton();
			ShaderManager::InstantiateSingleton();
			AudioManager::InstantiateSingleton();

			SetScene(s);
			SceneTransition();

			GameLoop();

		} catch (exception &e) {
			MessageBox(NULL,e.what(),"Exception thrown", MB_OK | MB_ICONEXCLAMATION);
		} catch (...) {
			MessageBox(NULL,"Anonymous exception caught.\nNo information available.",
					   "Exception thrown", MB_OK | MB_ICONEXCLAMATION);
		}

		// Clean up the scene
		if (scene) {
			delete scene;
		}

		ClearDeleteQueue();

		Input::ClearSingleton();
		ShaderManager::ClearSingleton();
		AudioManager::ClearSingleton();

		#ifdef _DEBUG
		if (commandline) {
			ConsoleReader::ShutDown();
		}
		#endif /* _DEBUG */

		renderWindow->KillWindow();
	}

	/*
	=====================
	GameControl::AddKeyListener
	=====================
	*/
	void GameControl::AddKeyListener(GameNode *node) {
		Input::GetSingleton()->AddKeyListener(node);
	}

	/*
	=====================
	GameControl::RemoveKeyListener
	=====================
	*/
	void GameControl::RemoveKeyListener(GameNode *node) {
		Input::GetSingleton()->RemoveKeyListener(node);
	}

	/*
	=====================
	GameControl::AddMouseListener
	=====================
	*/
	void GameControl::AddMouseListener(GameNode *node) {
		Input::GetSingleton()->AddMouseListener(node);
	}

	/*
	=====================
	GameControl::RemoveMouseListener
	=====================
	*/
	void GameControl::RemoveMouseListener(GameNode *node) {
		Input::GetSingleton()->RemoveMouseListener(node);
	}

	/*
	=====================
	GameControl::AddControlListener
	=====================
	*/
	void GameControl::AddControlListener(GameNode *n) {
		Input::GetSingleton()->AddControlListener(n);
	}

	/*
	=====================
	GameControl::RemoveControlListener
	=====================
	*/
	void GameControl::RemoveControlListener(GameNode *n) {
		Input::GetSingleton()->RemoveControlListener(n);
	}

	/*
	=====================
	GameControl::AddFrameListener
	=====================
	*/
	void GameControl::AddFrameListener(GameNode *node) {
		frameListeners.push_back(node);
	}

	/*
	=====================
	GameControl::RemoveFrameListener
	=====================
	*/
	void GameControl::RemoveFrameListener(GameNode *node) {
		for (unsigned int i=0; i<frameListeners.size(); i++) {
			if (frameListeners[i] == node) {
				frameListeners.erase(frameListeners.begin() + i);
			}
		}
	}

	/*
	=====================
	GameControl::Exit
	=====================
	*/
	void GameControl::Exit() {
		quit = true;
	}

	/*
	=====================
	GameControl::LimitFrame
	=====================
	*/
	void GameControl::LimitFrame(unsigned int maxFPS) {
		if (!maxFPS) {
			maxDelta = 0.f;
		} else {
			maxDelta = 1.f/(float)maxFPS;
		}
	}

	/*
	=====================
	GameControl::Pause
	=====================
	*/
	void GameControl::Pause() {
		if (!paused) {
			pauseLayer = scene->PauseLayer();

			if (pauseLayer) {
				scene->AddLayer(pauseLayer);
				pauseLayer->SetZOrder(-(INT_MAX-1));
				paused = true;
			}
		}
	}

	/*
	=====================
	GameControl::Unpause
	=====================
	*/
	void GameControl::Unpause() {
		if (paused && pauseLayer) {
			paused = false;
		}
	}

	/*
	=====================
	GameControl::SetWindowCreationData
	=====================
	*/
	void GameControl::SetWindowCreationData(WinStyle::CreationData data) {
		winData = data;
		winData.Prepare();

		renderWindow->SetCreationData(winData);
	}

	/*
	=====================
	GameControl::SetWindowStyle
	=====================
	*/
	void GameControl::SetWindowStyle(WinStyle::WinStyle style) {
		if (style != winData.winStyle) {
			winData.winStyle = style;
			renderWindow->SetCreationData(winData);
		}
	}

	/*
	=====================
	GameControl::SetWindowResolution
	=====================
	*/
	void GameControl::SetWindowResolution(int w, int h) {
		if (winData.resolution.x  != w || winData.resolution.y != h) {
			winData.resolution.x = (float)w;
			winData.resolution.y = (float)h;

			renderWindow->ResizeWindow(w, h);
		}
	}

	/*
	=====================
	GameControl::LowerLeftCorner
	=====================
	*/
	Vec2 GameControl::LowerLeftCorner() {
		switch (renderWindow->bpos) {
			case 1:	// VER
				return Vec2((float)renderWindow->bdim, 0.f);

			case 2: // HOR
				return Vec2(0.f, (float)renderWindow->bdim);

			case 0:
			default:
				return Vec2(0.f, 0.f);
		}
	}

	/*
	=====================
	GameControl::GetWindowScale
	=====================
	*/
	Vec2 GameControl::GetWindowScale() {
		return renderWindow->scale;
	}

	/*
	=====================
	GameControl::GetCoordinateFactor
	=====================
	*/
	Vec2 GameControl::GetCoordinateFactor() {
		return winData.coordinateSystem / renderWindow->ortho;
	}

	/*
	=====================
	GameControl::SetScene
	=====================
	*/
	void GameControl::SetScene(Scene *ns) {
		PimAssert(ns != NULL, "Error: Cannot set scene: NULL.");

		if (newScene) {
			delete newScene;
		}

		newScene = ns;
	}

	/*
	=====================
	GameControl::SceneTransition
	=====================
	*/
	void GameControl::SceneTransition() {
		if (newScene) {
			ShaderManager::GetSingleton()->ClearShaders();

			if (scene) {
				delete scene;
			}

			ClearDeleteQueue();

			scene = newScene;
			scene->LoadResources();

			newScene = NULL;

			// Discard the new (and too high) delta time
			CalculateDeltaTime();
		}
	}

	/*
	=====================
	GameControl::GameLoop
	=====================
	*/
	void GameControl::GameLoop() {
		MSG  msg;
		quit = false;
		ticks = clock();

		while (!quit) {
			while (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
				if (msg.message == WM_QUIT) {
					quit = true;
				} else {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}

			if (winHasMoved) {
				// Discard the delta time
				CalculateDeltaTime();
				winHasMoved = false;
			}

			// Get the DT
			float dt = CalculateDeltaTime();
			if (dt < maxDelta) {
				Sleep(DWORD(ceil((maxDelta-dt)*1000.f)));
				dt += CalculateDeltaTime();
			}

			if (!paused) {
				#ifdef _DEBUG
				ConsoleReader::GetSingleton()->Dispatch();
				ClearDeleteQueue();
				#endif /* _DEBUG */

				Input::GetSingleton()->Dispatch();
				ClearDeleteQueue();

				DispatchPrerender(dt);
			} else {
				// Dispatch input to all children of pauseLayer
				Input::GetSingleton()->DispatchPaused(pauseLayer);
				ClearDeleteQueue();

				DispatchPausedPreRender(dt);
			}

			ClearDeleteQueue();

			renderWindow->RenderFrame();

			AudioManager::GetSingleton()->UpdateSoundBuffers();

			// Was the game recently unpaused?
			if (pauseLayer && !paused) {
				scene->RemoveLayer(pauseLayer);
				pauseLayer = NULL;
			}

			// If a new scene has been set, transition it here
			SceneTransition();
		}

		ClearDeleteQueue();
	}

	/*
	=====================
	GameControl::DispatchPrerender
	=====================
	*/
	void GameControl::DispatchPrerender(float dt) {
		scene->Update(dt);

		vector<GameNode*> curFrameList = frameListeners;

		for (unsigned int i=0; i<curFrameList.size(); i++) {
			if (!curFrameList[i]->willDelete) {
				curFrameList[i]->Update(dt);
			}
		}
	}

	/*
	=====================
	GameControl::AddNodeToDelete
	=====================
	*/
	void GameControl::AddNodeToDelete(GameNode *node) {
		node->PrepareDeletion();
		delQueue.push_back(node);
	}

	/*
	=====================
	GameControl::ClearDeleteQueue
	=====================
	*/
	void GameControl::ClearDeleteQueue() {
		for each (GameNode *node in delQueue) {
			delete node;
		}

		delQueue.clear();
	}

	/*
	=====================
	GameControl::DispatchPausedPreRender
	=====================
	*/
	void GameControl::DispatchPausedPreRender(float dt) {
		// Iterate over ALL the children of pauseLayer
		DispatchPreRender_r(pauseLayer, dt);
	}

	/*
	=====================
	GameControl::DispatchPreRender_r
	=====================
	*/
	void GameControl::DispatchPreRender_r(GameNode *n, float dt) {
		n->Update(dt);

		for (unsigned int i=0; i<n->children.size(); i++) {
			DispatchPreRender_r(n->children[i], dt);
		}
	}

	/*
	=====================
	GameControl::CalculateDeltaTime
	=====================
	*/
	float GameControl::CalculateDeltaTime() {
		float dt = ((float)(clock() - ticks)) / CLOCKS_PER_SEC;
		ticks = clock();
		return dt;
	}
}