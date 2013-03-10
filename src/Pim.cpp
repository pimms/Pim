#include "PimInternal.h"
#include "pim.h"
#include "PimAssert.h"
#include "PimVec2.h"

using namespace Pim;

int NextPow2(int a) {
    int ret = 1;
	
    while (ret<a) {
		ret <<= 1;
	}
	
    return ret;
}

void WindCCW(Polygons *polygons) {
    // Ensure that all polygons are wound CCW
	for (unsigned j=0; j<polygons->size(); j++) {
		vector<Vec2> poly = (*polygons)[j];
		
        for (int i=0; i<3; i++) {
            int i2 = (i+1 < 3) ? (i+1) : (0);
            Vec2 edge = poly[i2] - poly[i];

            for (int j=0; j<3; j++) {
                if (j == i || j == i2)
                    continue;

                Vec2 r = poly[j] - poly[i];
                if (edge.Cross(r) <= 0.f) {
                    Vec2 tmp = poly[j];
                    poly[j] = poly[i];
                    poly[i] = tmp;
                }
            }
        }
    }
}

float RandF(float min, float max) {
	return ((float)rand()/(float)RAND_MAX) * (max-min) + min;
}
