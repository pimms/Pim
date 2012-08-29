#include "StdAfx.h"
#include "PimGameControl.h"

#include "Pim.h"

#include "PimException.h"
#include "PimRenderWindow.h"
#include "PimInput.h"
#include "PimGameNode.h"
#include "PimLayer.h"

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
				if (!HIWORD(wParam))
				{
					hasFocus = true;
					Input::getSingleton()->_gainedFocus();
				}
				else
				{
					hasFocus = false;
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
				GameControl::getRenderWindow()->resizeWindow(LOWORD(lParam), HIWORD(lParam));
				return 0;
		}

		return DefWindowProc(hWnd,msg,wParam,lParam);
	}


	// Singleton nullification
	GameControl* GameControl::singleton = NULL;


	GameControl::GameControl(std::string title, int w, int h)
	{
		PimAssert(singleton == NULL, "Only one GameControl instance can exist at a time.");

		singleton		= this;
		mWindowTitle	= title;
		mWindowWidth	= w;
		mWindowHeight	= h;
		layer			= NULL;
	}
	GameControl::~GameControl()
	{
		singleton = NULL;

		if (mWindow)	delete mWindow;
	}

	void GameControl::go(Layer *l)
	{
		try 
		{
			Input::instantiateSingleton();
			mWindow = new RenderWindow(mWindowTitle,mWindowWidth,mWindowHeight);

			setNewLayer(l);

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

	void GameControl::setNewLayer(Layer *l)
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

		while (!quit)
		{
			if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
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
		
			if (hasFocus)
			{	
				// Dispatch key and mouse events if required
				Input::getSingleton()->_dispatch();

				// Dispatch update calls
				dispatchUpdate();

				// Render dat frame
				mWindow->renderFrame();
			}
		}

		// Clean up the layer
		if (layer) delete layer;

		Input::clearSingleton();
		mWindow->killWindow();

		delete this;
	}
	void GameControl::dispatchUpdate()
	{
		float dt = calculateDeltaTime();

		for (unsigned int i=0; i<frameListeners.size(); i++)
		{
			frameListeners[i]->update(dt);
		}
	}

	float GameControl::calculateDeltaTime()
	{
		float dt = ((float)(clock() - ticks)) / CLOCKS_PER_SEC;
		ticks = clock();
		return dt;
	}
}