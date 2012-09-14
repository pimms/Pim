#include "Stdafx.h"

#include "PimException.h"

namespace Pim
{
	void PimAssert(bool statement, std::string desc)		
	{
		if (!statement) 
		{
			MessageBox(NULL, desc.c_str(), "PimAssertion failed!", MB_ICONEXCLAMATION | MB_OK);
			//throw new Exception(desc);
			exit(-1);
		}
	}
}