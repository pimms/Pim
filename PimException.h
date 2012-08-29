#pragma once

#include <stdlib.h>
#include <exception>
#include <string>

namespace Pim
{
	class Exception : public std::exception
	{
	public:
		Exception(std::string desc) : _desc(desc) {}

		std::string getDescription() 
		{
			return _desc;
		}

	private:
		Exception();
		Exception(Exception&);

		std::string _desc;
	};


	// Assertion
	void PimAssert(bool statement, std::string desc);
}