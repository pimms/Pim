
#include "Pim.h"

using namespace Pim;

class DemoLayer : public Layer {
public:
	Sprite *pimms;
	
	void LoadResources() {
		i = 0;
		
		pimms = new Sprite("pimms.png");
		AddChild(pimms);
		
		ListenFrame();
		ListenMouse();
		ListenController();
	}
	
	int i;
	void Update(float dt) {
		//printf("dt: %f\n", dt);
	}
	
	void OnMouseEvent(MouseEvent &event) {
		//printf("Position: %0.1f, %0.1f", event.GetPosition().x, event.GetPosition().y);
		pimms->position = event.GetPosition();
	}
	
	void OnControllerEvent(ControllerEvent &event) {
		if (event.IsKeyFresh(ControllerEvent::Xbox::X_A)) {
			printf("A fresh\n");
		}
		if (event.IsKeyFresh(ControllerEvent::Xbox::X_B)) {
			printf("B fresh\n");
		}
	}
};

class DemoScene : public Scene {
	void LoadResources() {
		DemoLayer *layer = new DemoLayer;
		AddLayer(layer);
		
		color = Color(1.f, 0.f, 0.f, 1.f);
	}
};

int main(int argc, char *argv[]) {
	GameControl gc;
	
	WinStyle::CreationData cd;
	cd.winTitle = "PIM OSX";
	cd.resolution = Vec2(600.f, 400.f);
	cd.forcedAspectRatio = true;
	cd.aspectRatio = 3.f/2.f;
	
	gc.Go(new DemoScene, cd);

	return 0;
}

/*
#include "SDL/SDL.h"
#include "SDL/SDL_main.h"
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>

void InitSDL() {
	const SDL_VideoInfo *video;
	
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
	
	SDL_JoystickEventState(SDL_ENABLE);
	
	video = SDL_GetVideoInfo();
	
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,            8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,          8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,           8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,          8);
	
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,          16);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,         32);
	
	SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE,      8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE,    8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE,     8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE,    8);
	
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,  1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,  2);
	
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 		1);
	
	SDL_SetVideoMode(600, 400, 32, SDL_OPENGL | SDL_HWSURFACE | SDL_RESIZABLE);
	
	SDL_WM_SetCaption("SDL TEST", NULL);
	
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.f, 0.f, 0.f, 0.f);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	
	glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, 600, 400);
	glOrtho(0, 600, 0, 400, 1.f, -1.f);
	
	glMatrixMode(GL_MODELVIEW);
}

void WindowResized(int w, int h) {
	printf("Resize: %i %i\n", w, h);
	
	SDL_SetVideoMode(w, h, 32, SDL_OPENGL | SDL_HWSURFACE | SDL_RESIZABLE);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glViewport(0, 0, w, h);
	glOrtho(0, w, 0, h, 1.f, -1.f);
	
	glMatrixMode(GL_MODELVIEW);
}

void Draw(int frames) {
	glLoadIdentity();
	glTranslatef(100, 100, 0);
	glColor3ub(frames%255, frames%255, 128);
	
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glBegin(GL_QUADS);
	glVertex2f(0.f, 0.f);
	glVertex2f(100.f, 0.f);
	glVertex2f(100.f, 100.f);
	glVertex2f(0.f, 100.f);
	glEnd();
	
	SDL_GL_SwapBuffers();
}

int main(int argc, char *argv[]) {
	
	InitSDL();
	
	bool quit = false;
	int frames = 0;

	while (!quit) {
		SDL_Event evt;
		while (SDL_PollEvent(&evt)) {
			switch (evt.type) {
				case SDL_QUIT:
					quit = true;
					break;
					
				case SDL_VIDEORESIZE:
					WindowResized(evt.resize.w, evt.resize.h);
					break;
			}
		}
		
		Draw(frames++);
	}
	
	
	
	
	return 0;
}
*/






















