// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>

#define _USE_MATH_DEFINES
#include <math.h>

#include <Windows.h>
#include <GL\glew.h>

#include "ft2build.h"
#include FT_FREETYPE_H

#include "png.h"

namespace Pim
{
	namespace Quality
	{
		typedef enum Quality
		{
			LOW,
			MED,
			HIGH,
		};
	}
}