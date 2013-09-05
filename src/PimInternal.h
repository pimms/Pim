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

	// MFC apps cannot include Windows.h. If you're using Pim in an
	// MFC project, define WIN_MFC in your project properties.
	#ifndef WIN_MFC
	#	include <Windows.h>
	#else
	#	include <afxwin.h>
	#endif

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
	#include <sys/time.h>

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
	#include <Pim/ft2build.h>
	#include <Pim/freetype/freetype.h>

	// LibPNG
	#include <libpng15/png.h>

	// Ogg Vorbis
	#include <Vorbis/codec.h>
	#include <Vorbis/vorbisfile.h>
#elif defined __gnu_linux__
	#include <sys/time.h>

	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glext.h>

	#include <SDL2/SDL.h>
	#include <SDL2/SDL_main.h>

	#include <ft2build.h>
	#include FT_FREETYPE_H

	// Fix for some compiler errors under
	// version 1.2.49.
	// Source: 
	// https://bugs.launchpad.net/ubuntu/+source/libpng/+bug/218409
	#define PNG_SKIP_SETJMP_CHECK
	#	include <png.h>
	#undef PNG_SKIP_SETJMP_CHECK

	#include <AL/al.h>
	#include <AL/alc.h>

	#include <vorbis/codec.h>
	#include <vorbis/vorbisfile.h>
#endif

using namespace std;

// Constants used for converting 
// between degrees and radians.
#define DEGTORAD ((float)M_PI/180.f)
#define RADTODEG (180.f/(float)M_PI)