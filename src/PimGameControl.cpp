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

#ifndef WIN32
	#include <unistd.h>
#endif

namespace Pim {

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

#ifdef WIN32
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
		
#elif defined __APPLE__
		modulePath = "/";
#endif
		
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

#if defined(_DEBUG) && defined(WIN32)
			if (commandline) {
				ConsoleReader::Begin();
			}
			printf("\n[PIM-version %s]\n", PIM_VERSION);
			printf("[OpenGL-version %s]\n\n", glGetString(GL_VERSION));
#endif /* _DEBUG && WIN32 */

			Input::InstantiateSingleton();
			ShaderManager::InstantiateSingleton();
			AudioManager::InstantiateSingleton();

			SetScene(s);
			SceneTransition();

			GameLoop();

		} catch (exception &e) {
			PimWarning(e.what(), "Exception Caught");
		} catch (string &s) {
			PimWarning(s.c_str(), "Exception Caught");
		} catch (...) {
			PimWarning("Anonymous exception caught.\nNo information available.",
					   "Exception thrown");
		}

		// Clean up the scene
		if (scene) {
			delete scene;
		}

		ClearDeleteQueue();

		Input::ClearSingleton();
		ShaderManager::ClearSingleton();
		AudioManager::ClearSingleton();

#if defined(_DEBUG) && defined(WIN32)
		if (commandline) {
			ConsoleReader::ShutDown();
		}
#endif /* _DEBUG && WIN32 */

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
		bool appRunning = true;
		ticks = clock();

		while (appRunning) {
			appRunning = HandleEvents();

			bool winHasMoved = false;
			if (winHasMoved) {
				// Discard the delta time
				CalculateDeltaTime();
				winHasMoved = false;
			}

			// Get the DT
			float dt = CalculateDeltaTime();
			if (dt < maxDelta) {
#ifdef WIN32
				Sleep(DWORD(ceil((maxDelta-dt)*1000.f)));
#else
				// wat.jpg
				//sleep(ceil((maxDelta-dt)*1000.f));
#endif
				//dt += CalculateDeltaTime();
			}

			if (!paused) {
#if defined(_DEBUG) && defined(WIN32)
				ConsoleReader::GetSingleton()->Dispatch();
				ClearDeleteQueue();
#endif /* _DEBUG && WIN32 */

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
	GameControl::HandleEvents

	Reads and dispatches SDL events.
	If the event SDL_QUIT is raised, 'false' is returned - otherwise, 
	true is returned.
	=====================
	*/
	bool GameControl::HandleEvents() {

		/*	Window movement-events are NOT dispatched in SDL 1.2.15.
		 *  When the window is moved, the app freezes and is unresponsive
		 *	until the window is released. 
		 *  
		 *	The app does not freeze until the application polls for events.
		 *	The current workaround is thus to measure the time it takes to
		 *	poll for events. If the polling takes more than 3 milliseconds, 
		 *  (normalky at 0.5-1.5ms) the "drag-time" is deducted from the 
		 *  next delta time.
		 */

		clock_t prepoll = clock();

		SDL_Event event;
		bool keepRunning = true;

		while (SDL_PollEvent(&event)) {

			switch (event.type) {
				case SDL_QUIT:
					keepRunning = false;
					break;

				case SDL_KEYDOWN:
					Input::GetSingleton()->KeyPressed(
						event.key.keysym.sym 
					);
					break;

				case SDL_KEYUP:
					Input::GetSingleton()->KeyReleased(
						event.key.keysym.sym
					);
					break;

				case SDL_MOUSEMOTION:
					Input::GetSingleton()->MouseMoved(
						event.motion.x, event.motion.y
					);
					break;

				case SDL_MOUSEBUTTONDOWN:
				{
					printf("MOUSE: %i\n", event.button.button);

					char mIdx = event.button.button;
					if (mIdx == 1 || mIdx == 3) {
						mIdx = (mIdx == 1) ? (0) : (1);
						Input::GetSingleton()->MousePressed(
							mIdx
						);
					}
					break;
				}

				case SDL_MOUSEBUTTONUP:
				{
					char mIdx = event.button.button;
					if (mIdx == 1 || mIdx == 3) {
						mIdx = (mIdx == 1) ? (0) : (1);
						Input::GetSingleton()->MouseReleased(
							mIdx
						);
					}
					break;
				}

				case SDL_VIDEORESIZE:
					renderWindow->ResizeWindow(
						event.resize.w,
						event.resize.h
					);
#ifndef WIN32
					ReloadTextures();
#endif
					break;

				case SDL_JOYAXISMOTION:
					Input::GetSingleton()->ControllerAxisMoved(
						event.jaxis.axis,
						event.jaxis.value
					);
					break;

				case SDL_JOYBUTTONDOWN:
					Input::GetSingleton()->ControllerButtonPressed(
						event.jbutton.button
					);
					break;

				case SDL_JOYBUTTONUP:
					Input::GetSingleton()->ControllerButtonReleased(
						event.jbutton.button
					);
					break;

				case SDL_JOYHATMOTION:
					/* The D-pad on the 360-controller is treated by SDL as a 
					 * "Joystick Hat", and the 'value' field in the event is a
					 * bitfield. As such, the bit values in 'value' are shifted
					 * 11 bits left (See ControllerEvent::Xbox enum). 
					 */
					short val = event.jhat.value << 11;
					for (int i=11; i<=14; i++) {
						if ((val & 1 << i) != 0) {
							Input::GetSingleton()->ControllerButtonPressed(
								val
							);
						} else {
							Input::GetSingleton()->ControllerButtonReleased(
								val
							);
						}
					}
					break;
			}
		}

		clock_t postpoll = clock();
		if (postpoll - prepoll > 3) {
			//ticks += (postpoll - prepoll);
		}

		return keepRunning;
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
		for (unsigned i=0; i<delQueue.size(); i++) {
			delete delQueue[i];
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

	/*
	==================
	GameControl::ReloadTextures
	==================
	*/
	void GameControl::ReloadTextures() {
		printf("\nReloading all textures...\n");
		
		if (scene) {
			scene->ReloadTextures();
		}

		if (paused && pauseLayer) {
			pauseLayer->ReloadTextures();
		}

		printf("Done!\n\n");
	}
}