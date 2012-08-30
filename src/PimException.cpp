#include "Stdafx.h"

#include "PimException.h"

namespace Pim
{
	void PimAssert(bool statement, std::string desc)		
	{
		if (!statement) 
			throw new Exception(desc); 
	}
}