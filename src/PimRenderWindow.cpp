#include "PimInternal.h"
#include "PimRenderWindow.h"

#include "PimGameControl.h"
#include "PimAssert.h"
#include "PimLayer.h"
#include "PimScene.h"

#include <stdlib.h>

namespace Pim {

	/*
	=====================
	RenderWindow::RenderWindow
	=====================
	*/
	RenderWindow::RenderWindow() {
		window = NULL;
		sdlWindow = true;
	}

	/*
	=====================
	RenderWindow::~RenderWindow
	=====================
	*/
	RenderWindow::~RenderWindow() {
		KillWindow();
	}

	/*
	=====================
	RenderWindow::GetOrtho
	=====================
	*/
	Vec2 RenderWindow::GetOrtho() const {
		return ortho;
	}

	/*
	=====================
	RenderWindow::GetOrthoOffset
	=====================
	*/
	Vec2 RenderWindow::GetOrthoOffset() const {
		return orthoOff;
	}

	/*
	=====================
	RenderWindow::PrintOpenGLErrors
	=====================
	*/
	void RenderWindow::PrintOpenGLErrors(string identifier) const {
		GLenum en = glGetError();
		if (en != GL_NO_ERROR) {
			cout<<"OpenGL error (" <<identifier <<"): ";

			switch (en) {
			case GL_INVALID_ENUM:
				cout<<"GL_INVALID_ENUM";
				break;
			case GL_INVALID_VALUE:
				cout<<"GL_INVALID_VALUE";
				break;
			case GL_INVALID_OPERATION:
				cout<<"GL_INVALID_OPERATION";
				break;
			case GL_OUT_OF_MEMORY:
				cout<<"GL_OUT_OF_MEMORY";
				break;
			case GL_STACK_UNDERFLOW:
				cout<<"GL_STACK_UNDERFLOW";
				break;
			case GL_STACK_OVERFLOW:
				cout<<"GL_STACK_OVERFLOW";
				break;
			default:
				cout<<"UNDEFINED GL ERROR";
			}

			cout<<"\n";
		}
	}

	/*
	=====================
	RenderWindow::SetupWindow
	=====================
	*/
	bool RenderWindow::SetupWindow(WinStyle::CreationData &data) {
		winData = data;
		sdlWindow = winData.createSDLWindow;

		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
			PimWarning("Failed to initialize SDL", "SDL Error");
			return false;
		}

		if (winData.createSDLWindow) {
			window = SDL_CreateWindow(	data.winTitle.c_str(),
										200, 200,
										data.resolution.x, data.resolution.y, 
										winData.GetWindowCreationFlags());
			if (!window) {
				PimWarning("Failed to create a window", "SDL Error");
				KillWindow();
				return false;
			}

			SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

			SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

#			ifdef _DEBUG
				/* Get the renderer information, ensure that
				 * SDL is compiled to use OpenGL.
				 */ 
				SDL_RendererInfo info;
				SDL_GetRendererInfo(renderer, &info);
				if (strcmp("direct3d", info.name) == 0) {
					SDL_Quit();
					printf("ERROR: SDL uses D3D!\n\nQUITTING!!!\n\n");
					system("pause");
					exit(1);
				}
#			endif

			SDL_JoystickEventState(SDL_ENABLE);

			SDL_GL_SetAttribute(SDL_GL_RED_SIZE,            8);
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,          8);
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,           8);
			SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,          8);
 
			SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,         32);
 
			SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE,      8);
			SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE,    8);
			SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE,     8);
			SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE,    8);
 
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,  1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,  2);
		
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 		1);
		} else {
#			ifdef WIN32
				window = SDL_CreateWindowFrom(winData.hWnd);
				SDL_SetWindowGrab(window, SDL_TRUE);
#			else 
				PimAssert(false, "Error: Under UNIX, a window must be created");
#			endif
		}

		// Create the window and set up the viewport to
		// match the resolution defined.
		ResizeWindow((int)winData.resolution.x, (int)winData.resolution.y);


#ifdef WIN32
	    // Initate GLEW
		GLenum res = glewInit();
		if (res != GLEW_OK) {
			printf("Glew error message: %i\n", res);
			PimWarning("Failed to initiate GLEW!", "GLEW Error");
			return false;
		}
#endif

	    return InitOpenGL();
	}

	/*
	=====================
	RenderWindow::KillWindow
	=====================
	*/
	void RenderWindow::KillWindow() {
		if (window) {
			SDL_DestroyWindow(window);
			window = NULL;
		}

		SDL_Quit();
	}

	/*
	=====================
	RenderWindow::SetWindowStyle
	=====================
	*/
	void RenderWindow::SetWindowStyle(WinStyle::WinStyle style) {
		if (style == WinStyle::WINDOWED) {
			SDL_SetWindowBordered(window, SDL_TRUE);
		} else {
			SDL_SetWindowBordered(window, SDL_FALSE);
			SDL_SetWindowFullscreen(window, 0);
		}
	}
	
	/*
	=====================
	RenderWindow::ResizeWindow
	=====================
	*/
	void RenderWindow::ResizeWindow(int nw, int nh) {
		if (nh == 0) {
			nh = 1;
		}

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glViewport(0,0,nw,nh);

		if (winData.forcedAspectRatio) {
			float rap = (float)nw/(float)nh;	// Real aspect ratio
	
			float rw = (float)nw;				// Real width
			float rh = (float)nh;				// Real height

			if (abs(rap - winData.aspectRatio) < 0.02f) { // 0.02
				bpos = NONE;
				bdim = 0;
			} else if (rap > winData.aspectRatio) {		// Too wide
				rw = nh * winData.aspectRatio;

				bpos = VER;
				bdim = (int)ceil((nw-rw)/2.f);
				orthoOff = Vec2((float)bdim, 0.f);
			} else if (rap < winData.aspectRatio) {	// Too tall
				rh = nw / winData.aspectRatio;

				bpos = HOR;
				bdim = (int)ceil((nh-rh)/2.f);
				orthoOff = Vec2(0.f, (float)bdim);
			}

			glOrtho((nw-rw)/-2.f, rw+(nw-rw)/2.f, (nh-rh)/-2.f, rh+(nh-rh)/2.f, 0, 1);
			ortho = Vec2(rw,rh);

		} else {
			glOrtho(0, nw, 0, nh, 0, 1);
			ortho = Vec2((float)nw,(float)nh);
			orthoOff = Vec2(0.f, 0.f);
			bpos = NONE;
		}

		glMatrixMode(GL_MODELVIEW);

		GameControl::GetSingleton()->actualWinWidth  = nw;
		GameControl::GetSingleton()->actualWinHeight = nh;

		scale = ortho / winData.renderResolution;
		
		glEnable(GL_TEXTURE_2D);
	}

	/*
	=====================
	RenderWindow::InitOpenGL
	=====================
	*/
	bool RenderWindow::InitOpenGL() {
		glEnable(GL_TEXTURE_2D);
		glShadeModel(GL_SMOOTH);
		glClearColor(0.f, 0.f, 0.f, 0.f);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);

		glMatrixMode(GL_MODELVIEW);

		return true;
	}

	/*
	=====================
	RenderWindow::SetCreationData
	=====================
	*/
	void RenderWindow::SetCreationData(WinStyle::CreationData &data) {
		if (data.winStyle != winData.winStyle) {
			SetWindowStyle(data.winStyle);
		}

		if (data.forcedAspectRatio	!= winData.forcedAspectRatio
		||	data.aspectRatio		!= winData.aspectRatio 
		||	data.renderResolution	!= winData.renderResolution
		||  data.resolution			!= winData.resolution) {
			ResizeWindow((int)data.resolution.x, (int)data.resolution.y);
		}

		winData = data;
	}

	/*
	=====================
	RenderWindow::RenderFrame
	=====================
	*/
	void RenderWindow::RenderFrame() {
#		ifdef _DEBUG
			PrintOpenGLErrors("PRERENDER FRAME (Should never occur)");
#		endif /* _DEBUG */

		// Clear screen with the top layer's color
		Scene *s = GameControl::GetScene();

		glClearColor(s->color.r,s->color.g,s->color.b,s->color.a);
		glClear(GL_COLOR_BUFFER_BIT);		// Clear the screen
		glClearColor(0.f, 0.f, 0.f, 0.f);

		glColor3ub(255,255,255);			// Default overlay color (white)

		glLoadIdentity();					// Reset The Current view Matrix

		s->DrawScene();						// Render the scene

		if (bpos == VER) {
			glColor4ub(0,0,0,255);
			int ww = GameControl::GetSingleton()->actualWinWidth;
			int wh = GameControl::GetSingleton()->actualWinHeight;

			glDisable(GL_TEXTURE_2D);
			glBegin(GL_QUADS);

				// Left
				glVertex2i(-bdim-5, 0);		
				glVertex2i(0, 0);
				glVertex2i(0, wh);			
				glVertex2i(-bdim-5, wh);

				// Right
				glVertex2i(ww-bdim+5, 0);
				glVertex2i(ww-bdim-bdim, 0);
				glVertex2i(ww-bdim-bdim, wh);
				glVertex2i(ww-bdim+5, wh);

			glEnd();
			glEnable(GL_TEXTURE_2D);
		} else if (bpos == HOR) {
			glColor4ub(0,0,0,255);
			int ww = GameControl::GetSingleton()->actualWinWidth;
			int wh = GameControl::GetSingleton()->actualWinHeight;

			glDisable(GL_TEXTURE_2D);
			glBegin(GL_QUADS);

				// Top
				glVertex2i(0,wh-bdim+5);
				glVertex2i(ww,wh-bdim+5);
				glVertex2i(ww,wh-bdim-bdim);
				glVertex2i(0,wh-bdim-bdim);

				// Bottom
				glVertex2i(0,-bdim-5);
				glVertex2i(ww,-bdim-5);
				glVertex2i(ww,0);
				glVertex2i(0,0);

			glEnd();
			glEnable(GL_TEXTURE_2D);
		}

		if (sdlWindow) {
			SDL_GL_SwapWindow(window);
		} else {
#ifdef WIN32
			//SwapBuffers(winData.devCtx);
			wglSwapLayerBuffers(winData.devCtx, WGL_SWAP_MAIN_PLANE);
#endif
		}
		
		#ifdef _DEBUG
		PrintOpenGLErrors("POSTRENDER FRAME");
		#endif /* _DEBUG */
	}
}