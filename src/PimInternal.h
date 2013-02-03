#pragma once

using namespace std;

#define PIM_VERSION "0.8.1"

// memory-leak tracing for debug builds
#ifdef _DEBUG
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
#include <cmath>

#define _USE_MATH_DEFINES
#include <math.h>

#include "PimWarning.h"

#ifdef WIN32
	// Threading
	#include <process.h>	// To be removed in the future
	//#include <omp.h>

	// windows and OpenGL (glew)
	#include <Windows.h>
	#include <GL\glew.h>

	// Direct sound
	#include <mmsystem.h>
	#include <dsound.h>

    // freetype
    #include "ft2build.h"
    #include FT_FREETYPE_H

    // libpng
    #include "png.h"

    // Ogg Vorbis
    #include "vorbis\codec.h"
    #include "vorbis\vorbisfile.h"
#elif defined __APPLE__
    // OpenGL
    #include <OpenGL/gl.h>

    // Freetype
    #include "ft2build.h"
    #include <freetype/freetype.h>

    // libpng
    #include <libpng15/png.h>

    // Vorbis
    #include <Vorbis/codec.h>
    #include <Vorbis/vorbisfile.h>
#endif

// Custom defines
#define DEGTORAD ((float)M_PI/180.f)
#define RADTODEG (180.f/(float)M_PI)

// Redefine M_PI as float
#ifdef M_PI
	#undef M_PI
	#define M_PI			3.14159265358979323846f
#endif