#pragma once

#define PIM_VERSION "0.6c"

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
#define _USE_MATH_DEFINES
#include <math.h>

// Threading
#include <process.h>

// windows and OpenGL (glew)
#include <Windows.h>
#include <GL\glew.h>

// freetype
#include "ft2build.h"
#include FT_FREETYPE_H

// libpng
#include "png.h"	

// Direct sound
#include <mmsystem.h>
#include <dsound.h>

// Ogg Vorbis
#include "vorbis\codec.h"
#include "vorbis\vorbisfile.h"