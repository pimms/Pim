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

#ifdef __APPLE__
	#include <unistd.h>
	#include <pthread.h>
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

		maxDelta		= 0.f;
		sleepNextFrame	= false;
		sleepTime		= 0.f;

#ifdef WIN32 /* Windows specific initialization */
		ticks 			= clock();
		
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
		
#elif defined __APPLE__ /* OSX specific initialization */
		struct timeval time;
		gettimeofday(&time, NULL);
		ticks = Tick(time.tv_sec) + Tick(time.tv_usec / 1000000.0);
		
		modulePath = ".";
		
		
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

			renderWindow = new RenderWindow();
			renderWindow->SetupWindow(data);

#if defined(_DEBUG) && defined(WIN32)
			if (commandline) {
				ConsoleReader::Begin();
			}
#endif /* _DEBUG && WIN32 */
			
			printf("\n[PIM-version %s]\n", PIM_VERSION);
			printf("[OpenGL-version %s]\n\n", glGetString(GL_VERSION));

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
		quit = false;

		while (!quit) {
			HandleEvents();

			// Get the DT
			float dt = CalculateDeltaTime();
			if (false && dt < maxDelta) {
#ifdef WIN32
				Sleep(DWORD(ceil((maxDelta-dt)*1000.f)));
				dt += CalculateDeltaTime();
#else
				usleep(ceil((maxDelta-dt)*1000000.f));
				dt += CalculateDeltaTime();
#endif
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
	=====================
	*/
	void GameControl::HandleEvents() {

		/*	Window movement-events are NOT dispatched in SDL 1.2.15.
		 *  When the window is moved, the app freezes and is unresponsive
		 *	until the window is released. 
		 *  
		 *	The app does not freeze until the application polls for events.
		 *	The current workaround is thus to measure the time it takes to
		 *	poll for events. If the polling takes more than 3 milliseconds, 
		 *  (normally at 0.5-1.5ms) the poll-time is deducted from the 
		 *  next delta time.
		 */

#ifdef WIN32
		clock_t prepoll = clock();
#endif
		
		SDL_Event event;

		while (SDL_PollEvent(&event)) {

			switch (event.type) {
				case SDL_QUIT:
					Exit();
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
					if (event.button.button >= 7) {
						break;
					}

					Input::GetSingleton()->MousePressed(
						event.button.button-1
					);
					break;
				}

				case SDL_MOUSEBUTTONUP:
				{
					Input::GetSingleton()->MouseReleased(
						event.button.button-1
					);
					break;
				}

				/* Subswitch */
				case SDL_WINDOWEVENT:
					switch (event.window.event) {
						case SDL_WINDOWEVENT_RESIZED:
							renderWindow->ResizeWindow(
								event.window.data1,
								event.window.data2
							);
							break;	
					}
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
						if ((val & (1 << i)) != 0) {
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

#ifdef WIN32
		clock_t postpoll = clock();
		if (postpoll - prepoll > 3) {
			ticks += (postpoll - prepoll);
		}
#endif
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

	Pre-render update calls are dispatched recursively. This dispatch
	method is only used when the game is paused. All nodes in the 
	"Paused node-tree" (children of 'pauseLayer') receives this update.
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
	 
	WINDOWS IMPLEMENTATION
	 
	The Windows implementation uses the clock()-function to
	calculate seconds since the application's launch.
	=====================
	*/
#ifdef WIN32
	float GameControl::CalculateDeltaTime() {
		clock_t  newTick = clock();
		
		float dt = float(newTick - ticks) / CLOCKS_PER_SEC;
		
		ticks = newTick;
		
		return dt;
	}
#endif
	
	/*
	=====================
	GameControl::CalculateDeltaTime
	
	OS X IMPLEMENTATION    (UNDER CONSTRUCTION)
	 
	The OSX implementation uses the <sys/time> library to calculate
	the delta time using the UNIX timestamp.
	 
	The delta time calculated from clock() by some fault wrong on OSX.
	This method is less efficient, but more precise.
	=====================
	*/
#ifdef __APPLE__	
	float GameControl::CalculateDeltaTime() {
		struct timeval time;
		gettimeofday(&time, NULL);
		
		Tick newTick = Tick(time.tv_sec) + Tick(time.tv_usec / 1000000.0);
		float dt = newTick - ticks;
		
		ticks = Tick(time.tv_sec) + Tick(time.tv_usec / 1000000.0);
		
		return dt;
	}
#endif

	/*
	==================
	GameControl::ReloadTextures
	==================
	*/
	void GameControl::ReloadTextures() {
#ifdef _DEBUG
		printf("Reloading all textures... ");
#endif

		if (scene) {
			scene->ReloadTextures();
		}

		if (paused && pauseLayer) {
			pauseLayer->ReloadTextures();
		}

#ifdef _DEBUG
		printf("Done!\n");
#endif
	}
}