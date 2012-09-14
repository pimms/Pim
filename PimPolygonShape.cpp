#include "Stdafx.h"

#include "PimVec2.h"
#include "PimPolygonShape.h"
#include "PimGameNode.h"
#include "PimException.h"

namespace Pim
{
	Line::Line(Vec2 &v1, Vec2 &v2)
	{
		p1 = v1;
		p2 = v2;

		// Calculate the normal

		// We know the vertices are wounded CCW. Hence, we know which direction the line goes in.
		if (v2.x > v1.x)
		{

		}
	}
	float Line::relativeDot(Vec2 &vec)
	{
		return normal.dot(shape->parent->position - vec);
	}


	PolygonShape::PolygonShape(Vec2 vertices[], int vertexCount)
	{
		PimAssert(vertexCount > 2, "Error: A shape must have 3 or more vertices!");

		int i=0;
		while (i<vertexCount-1)
			lines.push_back(new Line(vertices[i], vertices[++i]));
		lines.push_back(new Line(vertices[i], vertices[0]));
	}
	PolygonShape::~PolygonShape()
	{
		for (unsigned int i=0; i<lines.size(); i++)
		{
			delete lines[i];
		}
	}

	bool PolygonShape::shapeContains(Vec2 &vec)
	{
		for (unsigned int i=0; i<lines.size(); i++)
		{
			if (lines[i]->relativeDot(vec) >= 0.f)
				return false;
		}
		return true;
	}
}