// pims.h

#pragma once

#include <string>

#include <Windows.h>
#include <GL\GL.H>
#include <GL\GLU.H>

// Commonly used public functions
LPCWSTR strToLPCWSTR(const std::string& s);				// string to LPCWSTR conversion

// Engine headers
#include "PVec2.h"
#include "GameControl.h"
