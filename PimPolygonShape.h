#pragma once

#include "Stdafx.h"

namespace Pim
{
	// Forward declarations
	class GameNode;
	class PolygonShape;
	class Vec2;

	class Line
	{
	public:
		Line(Vec2 &v1, Vec2 &v2);

		// Returns the relative dot product between the normal and the provided vector.
		float relativeDot(Vec2 &vec);

		Vec2 p1, p2, normal;

	private:
		friend class PolygonShape;

		PolygonShape *shape;
	};


	class PolygonShape
	{
	public:
		// THE VERTICES MUST BE WOUND COUNTER CLOCKWISE, AND THE SHAPE MUST BE CONCAVE.
		PolygonShape(Vec2 *vertices, int vertexCount);
		~PolygonShape();

		// Returns true if the provided point is contained by the  shape
		bool shapeContains(Vec2 &vec);

	private:
		friend class Line;
		friend class GameNode;

		PolygonShape()						{}
		PolygonShape(const PolygonShape&)	{}

		GameNode *parent;
		std::vector<Line*>	lines;
	};
}