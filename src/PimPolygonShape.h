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
	class CollisionManager;

	class Line
	{
	public:
		Line(Vec2 &v1, Vec2 &v2, PolygonShape *p);

		/* 
			THE SC PARAMETER IS THE SCALE OF THE LINE COORDINATES ITSELF.
			THE ROTATION IN WORLD SPACE IS PERFORMED WITHOUT THIS SCALE.
		*/
		Vec2 getP1(Vec2 &sc = Vec2(1,1));			// Returns the first point
		Vec2 getP2(Vec2 &sc = Vec2(1,1));			// Returns the second point
		Vec2 getNormal(Vec2 &sc = Vec2(1,1));		// Returns the normal
		Vec2 getNormalEnd(Vec2 &sc = Vec2(1,1));	// Returns mid+normal transformed
		Vec2 getMid(Vec2 &sc = Vec2(1,1));			// Returns the center of the line

		// Returns the relative dot product between the normal and the provided vector.
		float relativeDot(Vec2 &vec, Vec2 &sc = Vec2(1,1));

		// return relativeDot(vec) >= 0.f;
		bool isFacing(Vec2 &vec, Vec2 &sc = Vec2(1,1));

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
		// THE VERTICES MUST BE WOUND COUNTER CLOCKWISE, AND THE SHAPE MUST BE CONVEX.
		PolygonShape(Vec2 vertices[], int vertexCount, GameNode *par);
		~PolygonShape();

		// Returns true if the provided point is contained by the  shape
		bool shapeContains(Vec2 &vec);

		Vec2 center();

		// Public for the sake of publicness. Don't edit manually, you beast!
		std::vector<Line*>	lines;

	private:
		friend class Line;
		friend class GameNode;
		friend class Sprite;
		friend class LightingSystem;
		friend class CollisionManager;

		PolygonShape()						{parent=NULL;}
		PolygonShape(const PolygonShape&)	{parent=NULL;}

		void debugDraw();

		void projectPolygon(Vec2 axis, float &min, float &max, Vec2 offset=Vec2(0.f,0.f));

		GameNode *parent;
	};
}