#pragma once

#include <stdlib.h>
#include <exception>
#include <cassert>
#include <string>

namespace Pim
{
	// Assertion

#ifdef _DEBUG
void PimAssert(bool expr, std::string desc);
#else
#define PimAssert(_X_, _Y_) ;
#endif
}