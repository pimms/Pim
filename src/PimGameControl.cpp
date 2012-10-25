#include "StdAfx.h"
#include "PimGameControl.h"

#include "PimException.h"
#include "PimRenderWindow.h"
#include "PimInput.h"
#include "PimGameNode.h"
#include "PimLayer.h"
#include "PimShaderManager.h"
#include "PimCollisionManager.h"
#include "PimAudioManager.h"

#include <iostream>
#include <ctime>

namespace Pim
{
	// Global variables only accessible from this file. Used to
	// to store values from the callback function.
	bool hasFocus = false;

	// Callback for window events
	LRESULT	CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
			case WM_ACTIVATE:
				if (wParam)
				{
					hasFocus = true;
					if (Input::getSingleton())
						Input::getSingleton()->_gainedFocus();
				}
				else
				{
					hasFocus = false;
					if (Input::getSingleton())
						Input::getSingleton()->_lostFocus();
				}
				return 0;

			case WM_SYSCOMMAND:
				switch (wParam)
				{
					case SC_SCREENSAVE:
					case SC_MONITORPOWER:
					return 0;
				}
				break;

			case WM_COMMAND:
				std::cout<< "WM_COMMAND\n";
				return 0;

			case WM_CLOSE:
				PostQuitMessage(0);
				return 0;

			case WM_KEYDOWN:
				//std::cout<<"Key: " <<wParam <<"\n";
				Input::getSingleton()->_keyPressed(wParam);
				return 0;

			case WM_KEYUP:
				Input::getSingleton()->_keyReleased(wParam);
				return 0;

			// MOUSE MOVE
			case WM_MOUSEMOVE:
				//std::cout<<"Move: " <<LOWORD(lParam) <<", " <<HIWORD(lParam) <<"\n";
				Input::getSingleton()->_mouseMoved(LOWORD(lParam),HIWORD(lParam));
				return 0;

			// LEFT MOUSE
			case WM_LBUTTONDOWN:
				Input::getSingleton()->_mousePressed(0);
				return 0;
			case WM_LBUTTONUP:
				Input::getSingleton()->_mouseReleased(0);
				return 0;

			// RIGHT MOUSE
			case WM_RBUTTONDOWN:
				Input::getSingleton()->_mousePressed(1);
				return 0;
			case WM_RBUTTONUP:
				Input::getSingleton()->_mouseReleased(1);
				return 0;

			case WM_SIZE:
				GameControl::getSingleton()->setWindowResolution(LOWORD(lParam),HIWORD(lParam));
				return 0;
		}

		return DefWindowProc(hWnd,msg,wParam,lParam);
	}


	// Singleton nullification
	GameControl* GameControl::singleton = NULL;


	GameControl::GameControl()
	{
		PimAssert(singleton == NULL, "Only one GameControl instance can exist at a time.");

		singleton		= this;
		layer			= NULL;

		// Get the module path
		char path[260] = { '\0' };
		GetModuleFileName(NULL, path, 260);

		int lastSlash = 0;
		for (int i=0; i<260; i++)
			if (path[i] == '\\' || path[i] == '/')
				lastSlash = i;
			
		for (int i=lastSlash+1; i<260; i++)
			path[i] = '\0';

		modulePath = path;
		std::cout<<"Working directory:\n" <<modulePath <<"\n";
	}
	GameControl::~GameControl()
	{
		singleton = NULL;

		if (renderWindow)	delete renderWindow;
	}

	std::string GameControl::getModulePath()
	{
		return singleton->modulePath;
	}

	void GameControl::go(Layer *l, WinStyle::CreationData data)
	{
		try 
		{
			winData = data;
			winData.prepare();

			renderWindow = new RenderWindow(data);
			renderWindow->createWindow(data);

			Input::instantiateSingleton();
			ShaderManager::instantiateSingleton();
			CollisionManager::instantiateSingleton();
			AudioManager::instantiateSingleton();

#ifdef _DEBUG
		std::cout<<"\n[OpenGL version " <<glGetString(GL_VERSION) <<"]\n\n"; 
#endif

			setLayer(l);

			gameLoop();
		} 
		catch (Exception &e) 
		{
			MessageBox(NULL,e.getDescription().c_str(),"Exception thrown", MB_OK | MB_ICONEXCLAMATION);
		}
		catch (std::exception &e)
		{
			MessageBox(NULL,e.what(),"Exception thrown", MB_OK | MB_ICONEXCLAMATION);
		}
		catch (...)
		{
			MessageBox(NULL,"Anonymous exception caught.\nNo information available.",
				"Exception thrown", MB_OK | MB_ICONEXCLAMATION);
		}

		// Clean up the layer
		if (layer) delete layer;

		Input::clearSingleton();
		ShaderManager::clearSingleton();
		CollisionManager::clearSingleton();
		AudioManager::clearSingleton();

		renderWindow->killWindow();
	}

	void GameControl::addKeyListener(GameNode *node)
	{
		Input::getSingleton()->addKeyListener(node);
	}
	void GameControl::removeKeyListener(GameNode *node)
	{
		Input::getSingleton()->removeKeyListener(node);
	}

	void GameControl::addMouseListener(GameNode *node)
	{
		Input::getSingleton()->addMouseListener(node);
	}
	void GameControl::removeMouseListener(GameNode *node)
	{
		Input::getSingleton()->removeMouseListener(node);
	}

	void GameControl::addFrameListener(GameNode *node)
	{
		frameListeners.push_back(node);
	}
	void GameControl::removeFrameListener(GameNode *node)
	{
		for (unsigned int i=0; i<frameListeners.size(); i++)
		{
			if (frameListeners[i] == node)
				frameListeners.erase(frameListeners.begin() + i);
		}
	}

	void GameControl::setWindowCreationData(WinStyle::CreationData data)
	{
		winData = data;
		winData.prepare();

		renderWindow->setCreationData(winData);
	}
	void GameControl::setWindowStyle(WinStyle::WinStyle style)
	{
		if (style != winData.winStyle)
		{
			winData.winStyle = style;

			renderWindow->setCreationData(winData);
		}
	}
	void GameControl::setWindowResolution(int w, int h)
	{
		if (winData.resolution.x  != w || winData.resolution.y != h)
		{
			winData.resolution.x = w;
			winData.resolution.y = h;

			renderWindow->resizeWindow(w, h);
		}
	}

	Vec2 GameControl::lowerLeftCorner()
	{
		switch (renderWindow->bpos)
		{
		case 1:	// VER
			return Vec2((float)renderWindow->bdim, 0.f);

		case 2: // HOR
			return Vec2(0.f, (float)renderWindow->bdim);

		case 0: default:
			return Vec2(0.f, 0.f);
		}
	}
	Vec2 GameControl::windowScale()
	{
		return renderWindow->scale;
	}
	Vec2 GameControl::coordinateFactor()
	{
		return winData.coordinateSystem / renderWindow->ortho;
	}

	void GameControl::setLayer(Layer *l)
	{
		PimAssert(l != NULL, "Error: Cannot set layer: NULL.");

		if (layer)
			delete layer;

		layer = l;
		layer->_topLevelNode();
		layer->loadResources();
	}

	void GameControl::gameLoop() 
	{
		MSG  msg;
		bool quit = false;
		ticks = clock();

		while (!quit)
		{
			while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					quit = true;
				}
				else
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		
			// Dispatch key and mouse events 
			Input::getSingleton()->_dispatch();

			// Dispatch update calls
			dispatchPrerender();

			// Render the frame - post render calls are made by renderWindow
			renderWindow->renderFrame();

			// Dispatch postrender calls
			dispatchPostrender();
		}
	}
	void GameControl::dispatchPrerender()
	{
		float dt = calculateDeltaTime();

		for (unsigned int i=0; i<frameListeners.size(); i++)
		{
			frameListeners[i]->update(dt);
		}
	}
	void GameControl::dispatchPostrender()
	{
		for (unsigned int i=0; i<frameListeners.size(); i++)
		{
			frameListeners[i]->postFrame();
		}
	}

	float GameControl::calculateDeltaTime()
	{
		float dt = ((float)(clock() - ticks)) / CLOCKS_PER_SEC;
		ticks = clock();
		return dt;
	}
}