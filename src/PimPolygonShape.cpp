#include "Stdafx.h"

#include "PimSprite.h"
#include "PimVec2.h"
#include "PimPolygonShape.h"
#include "PimGameNode.h"
#include "PimException.h"
#include "PimLightingSystem.h"

namespace Pim
{
	Line::Line(Vec2 &v1, Vec2 &v2, PolygonShape *p)
	{
		p1		= v1;
		p2		= v2;
		mid		= (p1 + p2) / 2.f;
		shape	= p;

		float a = (v2-v1).angleBetween(Vec2(0.f,1.f));
		
		// Modify 'a' if necessary 
		if (v2.x > v1.x)
		{
			if (a > 0.f)
				a = 360.f - a;
		}
		else if (v1.x > v2.x)
		{
			if (a < 0.f)
				a = 360.f - a;
		}

		normal = Vec2(cosf(a*(M_PI/180.f)), sinf(a*(M_PI/180.f)));

	}

	Vec2 Line::getP1()
	{
		return p1.rotateAroundPoint(shape->parent->getLayerPosition(),
			shape->parent->rotation);
	}
	Vec2 Line::getP2()
	{
		return p2.rotateAroundPoint(shape->parent->getLayerPosition(),
			shape->parent->rotation);
	}
	Vec2 Line::getNormal()
	{
		return normal.rotateDegrees(shape->parent->rotation);
	}
	Vec2 Line::getNormalEnd()
	{
		return (mid+normal*10.f).rotateAroundPoint(shape->parent->getLayerPosition(),
			shape->parent->rotation);
	}
	Vec2 Line::getMid()
	{
		return mid.rotateAroundPoint(shape->parent->getLayerPosition(),
			shape->parent->rotation);
	}

	float Line::relativeDot(Vec2 &vec)
	{
		return getNormal().dot(vec-getMid());
	}
	bool Line::isFacing(Vec2 &vec)
	{
		return relativeDot(vec) >= 0.f;
	}


	PolygonShape::PolygonShape(Vec2 vertices[], int vertexCount, GameNode *par)
	{
		PimAssert(vertexCount > 2, "Error: A shape must have 3 or more vertices!");
		PimAssert(par, "Error: A shape must be provided a game node parent!");

		parent = par;

		int i=0;
		while (i<vertexCount-1)
		{
			Vec2 *v1 = &vertices[i];
			Vec2 *v2 = &vertices[++i];
			lines.push_back(new Line(*v1, *v2, this));
		}
		lines.push_back(new Line(vertices[i], vertices[0], this));
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

	void PolygonShape::debugDraw()
	{
		/*
			The matrix should at this point be translated to the parent's
			position, so there's no need to use the getter methods on the
			line's points - they will be drawn relative in both rotation
			and position of the parent regardless.
		*/
		glDisable(GL_TEXTURE_2D);

		glLineWidth(2.f);
		glBegin(GL_LINES);
		for (unsigned int i=0; i<lines.size(); i++)
		{
			// The line itself
			glColor4ub(0,0,0,255);
			glVertex2f(lines[i]->p1.x, lines[i]->p1.y);
			glVertex2f(lines[i]->p2.x, lines[i]->p2.y);

			// The normal
			glColor4ub(255,0,0,255);
			Vec2 p = lines[i]->mid + lines[i]->normal*10.f;
			glVertex2f(lines[i]->mid.x, lines[i]->mid.y);
			glVertex2f(p.x, p.y);
		}
		glEnd();

		glEnable(GL_TEXTURE_2D);
	}
}