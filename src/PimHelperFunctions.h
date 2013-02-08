#pragma once

namespace Pim {
	class Polygons;
}

/*
	All functions declared in this header is defined in Pim.cpp
*/

// Returns the next power of 2
int NextPow2(int a);

// Re-wind all the polygons in the Polygon-structure to be ordered CCW.
void WindCCW(Pim::Polygons *poly);

// Returns a random float in range
float RandF(float min, float max);