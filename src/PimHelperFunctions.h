#pragma once

namespace Pim { class Polygons; }

// Returns the next power of 2
int nextPow2(int a);

// Re-wind all the polygons in the Polygon-structure to be ordered CCW.
void windCCW(Pim::Polygons *poly);