#pragma once

#define PIM_VERSION "0.9 alpha"

// memory-leak tracing for debug builds
#if defined(_DEBUG) && defined(WIN32)
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

// C++ standard library
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#include <map>

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>


#ifdef WIN32
	// Threading
	#include <process.h>	// To be removed in the future
	//#include <omp.h>

	// windows and OpenGL (glew)
	#include <Windows.h>
	#include <GL\glew.h>

	// SDL
	#include "SDL\SDL.h"
	#include "SDL\SDL_main.h"

	// OpenAL
	#include "al.h"
	#include "alc.h"

	// Freetype
	#include "ft2build.h"
	#include FT_FREETYPE_H

	// LibPNG
	#include "png.h"

	// Ogg Vorbis
	#include "vorbis\codec.h"
	#include "vorbis\vorbisfile.h"
#elif defined __APPLE__
	// OpenGL
	#include <OpenGL/OpenGL.h>
	#include <OpenGL/gl3.h>
	#include <OpenGL/gl3ext.h>
	#include <OpenGL/gl.h>

	// SDL
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_main.h>

	// OpenAL
	#include <OpenAL/al.h>
	#include <OpenAL/alc.h>

	// Freetype
	#include "ft2build.h"
	#include "freetype/freetype.h"

	// LibPNG
	#include <libpng15/png.h>

	// Ogg Vorbis
	#include <Vorbis/codec.h>
	#include <Vorbis/vorbisfile.h>
#endif

using namespace std;

// Custom defines
#define DEGTORAD ((float)M_PI/180.f)
#define RADTODEG (180.f/(float)M_PI)

// Redefine M_PI as float
#ifdef M_PI
	#undef M_PI
	#define M_PI			3.14159265358979323846f
#endif