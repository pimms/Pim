// This is the main LIB file.
#include "stdafx.h"
#include "pim.h"
#include "PimException.h"
#include "PimVec2.h"



int nextPow2(int a)
{
	int ret = 1;
	while (ret<a) ret <<= 1;
	return ret;
}