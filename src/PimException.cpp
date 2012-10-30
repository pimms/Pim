#include "PimInternal.h"

#include "PimException.h"

namespace Pim
{
#ifdef _DEBUG
	void PimAssert(bool statement, std::string desc)		
	{
		if (!statement) 
		{
			MessageBox(NULL, desc.c_str(), "PimAssertion failed!", MB_ICONEXCLAMATION | MB_OK);
			//throw new Exception(desc);
			exit(-1);
		}
	}
#else
#define PimAssert(_NULL_,_VOID_) ;
#endif
}