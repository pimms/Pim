#pragma once

#include <stdlib.h>
#include <exception>
#include <string>

// Forward declarations
LPCWSTR strToLPCWSTR(const std::string& s);

class PimsException : public std::exception
{
public:
	PimsException(std::string desc) : _desc(desc) {}
	LPCWSTR getDescription() 
	{
		return strToLPCWSTR(_desc);
	}

private:
	PimsException();
	PimsException(PimsException&);

	std::string _desc;
};