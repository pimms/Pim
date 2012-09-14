#pragma once

#include "Stdafx.h"

namespace Pim
{
	// Forward declarations
	class GameNode;
	class Sprite;
	class PolygonShape;
	class Vec2;
	class LightingSystem;

	class Line
	{
	public:
		Line(Vec2 &v1, Vec2 &v2, PolygonShape *p);

		Vec2 getP1();			// Returns the first point
		Vec2 getP2();			// Returns the second point
		Vec2 getNormal();		// Returns the normal
		Vec2 getNormalEnd();	// Returns mid+normal transformed
		Vec2 getMid();			// Returns the center of the line

		// Returns the relative dot product between the normal and the provided vector.
		float relativeDot(Vec2 &vec);

		// return relativeDot(vec) >= 0.f;
		bool isFacing(Vec2 &vec);

	private:
		friend class PolygonShape;

		Line()				{shape=NULL;}
		Line(const Line&)	{shape=NULL;}

		PolygonShape *shape;

		Vec2 p1, p2, mid, normal;
	};


	class PolygonShape
	{
	public:
		// THE VERTICES MUST BE WOUND COUNTER CLOCKWISE, AND THE SHAPE MUST BE CONCAVE.
		PolygonShape(Vec2 vertices[], int vertexCount, GameNode *par);
		~PolygonShape();

		// Returns true if the provided point is contained by the  shape
		bool shapeContains(Vec2 &vec);

	private:
		friend class Line;
		friend class GameNode;
		friend class Sprite;
		friend class LightingSystem;

		PolygonShape()						{parent=NULL;}
		PolygonShape(const PolygonShape&)	{parent=NULL;}

		void debugDraw();

		GameNode *parent;
		std::vector<Line*>	lines;
	};
}