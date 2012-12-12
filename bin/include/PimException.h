#pragma once

#include <stdlib.h>
#include <exception>
#include <cassert>
#include <string>
#include <sstream>

namespace Pim
{
	// Assertion

#ifdef _DEBUG
/*void PimAssert(bool expr, std::string desc);*/
#define PimAssert(_EXPR,_DESC)										\
	if (!(_EXPR))													\
	{																\
		std::string __desc__ = _DESC;								\
		std::stringstream ss; ss<<__LINE__;							\
		__desc__.append("\nFile: "); __desc__.append(__FILE__);		\
		__desc__.append("\nLine: "); __desc__.append(ss.str());		\
		throw std::exception(__desc__.c_str());						\
	}															
#else
#define PimAssert(_X_, _Y_) ;
#endif
}