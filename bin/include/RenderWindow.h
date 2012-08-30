#pragma once

#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <GL/GLAux.h>

#include <string>

class GameControl;

class RenderWindow
{
public:
	RenderWindow(std::string title, int w, int h);
	~RenderWindow();

	void resizeWindow(int wnew, int hnew);

private:
	friend class GameControl;

	static RenderWindow* singleton;

	bool createWindow(std::string &title, int w, int h, int bits);
	bool initOpenGL();

	void killWindow();

	void renderFrame();

	
	HDC			hDC;		// Private device context
	HGLRC		hRC;		// Rendering context
	HWND		hWnd;		// Window handle
	HINSTANCE	hInstance;	// Application instance
};

