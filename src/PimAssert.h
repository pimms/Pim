#pragma once

#include <stdlib.h>
#include <exception>
#include <string>
#include <sstream>

/*
==================
PimAssert
 
If the provided expression is false, an exception is thrown
with the provided description.
==================
*/
#ifdef _DEBUG		// Only enable assertion in debug builds

#ifdef WIN32
	#define PimAssert(_EXPR,_DESC)										\
		if (!(_EXPR))													\
		{																\
			string __desc__ = _DESC;									\
			stringstream ss; ss<<__LINE__;								\
			__desc__.append("\nFile: "); __desc__.append(__FILE__);		\
			__desc__.append("\nLine: "); __desc__.append(ss.str());		\
			throw exception(__desc__.c_str());							\
		}
#else
	#define PimAssert(_EXPR,_DESC)										\
		if (!(_EXPR))													\
		{																\
			string __desc__ = _DESC;									\
			stringstream ss; ss<<__LINE__;								\
			__desc__.append("\nFile: "); __desc__.append(__FILE__);		\
			__desc__.append("\nLine: "); __desc__.append(ss.str());		\
			throw __desc__;												\
		}
#endif

#else				// Define as empty statement in release builds

	#define PimAssert(_X_, _Y_) ;

#endif


/*
==================
PimWarning
 
Depending on the platform, this macro will either
display a warning-window, or print an error message to the
console window.
==================
*/
#ifdef WIN32

	#define PimWarning(_DESC, _TITLE)									\
		MessageBox(NULL, _DESC, _TITLE, MB_ICONEXCLAMATION | MB_OK);	\

#else

	#define PimWarning(_DESC, _TITLE) 									\
		printf("\n[WARNING]: %s\n%s\n\n", _TITLE, _DESC);				\

#endif














