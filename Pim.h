// pim.h

#pragma once

#include <string>

#include <Windows.h>
#include <GL\GL.H>
#include <GL\GLU.H>

// Commonly used public functions
namespace Pim 
{
	LPCWSTR strToLPCWSTR(const std::string& s);				// string to LPCWSTR conversion
}

// Engine headers
#include "PimVec2.h"
#include "PimGameControl.h"
#include "PimGameNode.h"
#include "PimLayer.h"
