#pragma once

#include <stdlib.h>
#include <exception>
#include <string>

namespace Pim
{
	// Forward declarations
	LPCWSTR strToLPCWSTR(const std::string& s);

	// Assertion
	#define PimAssert(_s,_dsc)						\
		if (!_s) throw new Exception(_dsc); 

	class Exception : public std::exception
	{
	public:
		Exception(std::string desc) : _desc(desc) {}
		LPCWSTR getDescription() 
		{
			return strToLPCWSTR(_desc);
		}

	private:
		Exception();
		Exception(Exception&);

		std::string _desc;
	};
}