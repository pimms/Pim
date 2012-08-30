#pragma once

#include "pims.h"

// Forward declarations
class RenderWindow;

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
	static PVec2 getWindowSize()				
				{ return PVec2((float)getWindowWidth(),(float)getWindowHeight()); }

	GameControl(std::string title, int w, int h);
	~GameControl();

	void go();

	// Internal use  

private:
	static GameControl* singleton;

	void gameLoop();

	RenderWindow	*mWindow;
	std::string		mWindowTitle;
	int				mWindowWidth;
	int				mWindowHeight;
};

