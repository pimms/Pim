// This is the main LIB file.
#include "PimInternal.h"
#include "pim.h"
#include "PimAssert.h"
#include "PimVec2.h"



int nextPow2(int a)
{
	int ret = 1;
	while (ret<a) ret <<= 1;
	return ret;
}

void windCCW(Pim::Polygons *polygons)
{
	// Ensure that all polygons are wound CCW
	for each (auto poly in *polygons)
	{
		for (int i=0; i<3; i++)
		{
			int i2 = (i+1 < 3) ? (i+1) : (0);
			Pim::Vec2 edge = poly[i2] - poly[i];

			for (int j=0; j<3; j++)
			{
				if (j == i || j == i2)
					continue;

				Pim::Vec2 r = poly[j] - poly[i];
				if (edge.cross(r) <= 0.f)
				{
					Pim::Vec2 tmp = poly[j];
					poly[j] = poly[i];
					poly[i] = tmp;
				}
			}
		}
	}
}