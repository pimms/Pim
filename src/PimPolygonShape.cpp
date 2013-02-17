#include "PimInternal.h"

#include "PimSprite.h"
#include "PimVec2.h"
#include "PimPolygonShape.h"
#include "PimGameNode.h"
#include "PimAssert.h"
#include "PimLightingSystem.h"

namespace Pim {
	/*
	=====================
	Line::Line
	=====================
	*/
	Line::Line(const Vec2 &v1, const Vec2 &v2, const PolygonShape *p) {
		p1		= v1;
		p2		= v2;
		mid		= (p1 + p2) / 2.f;
		shape	= p;

		float a = (v2-v1).AngleBetween(Vec2(0.f,1.f));
		normal = Vec2(cosf(a*DEGTORAD), sinf(a*DEGTORAD));

		if ((v1.x > v2.x && v1.y > v2.y) ||
			(v1.x < v2.x && v1.y < v2.y)) {
			normal.x *= -1.f;
		}
	}

	/*
	=====================
	Line::GetP1
	=====================
	*/
	Vec2 Line::GetP1(const Vec2 &sc) const {
		return (p1*sc).RotateAroundPoint(shape->GetParent()->GetLayerPosition(),
										 shape->GetParent()->rotation);
	}

	/*
	=====================
	Line::GetP2
	=====================
	*/
	Vec2 Line::GetP2(const Vec2 &sc) const {
		return (p2*sc).RotateAroundPoint(shape->GetParent()->GetLayerPosition(),
										 shape->GetParent()->rotation);
	}

	/*
	=====================
	Line::GetNormal
	=====================
	*/
	Vec2 Line::GetNormal(const Vec2 &sc) const {
		return normal.RotateDegrees(shape->parent->rotation);
	}

	/*
	=====================
	Line::GetNormalEnd
	=====================
	*/
	Vec2 Line::GetNormalEnd(const Vec2 &sc) const {
		return (mid+normal*10.f).RotateAroundPoint(shape->GetParent()->GetLayerPosition(),
													shape->GetParent()->rotation);
	}

	/*
	=====================
	Line::GetMid
	=====================
	*/
	Vec2 Line::GetMid(const Vec2 &sc) const {
		return (mid*sc).RotateAroundPoint(shape->GetParent()->GetLayerPosition(),
											shape->GetParent()->rotation);
	}

	/*
	=====================
	Line::RelativeDot
	=====================
	*/
	float Line::RelativeDot(const Vec2 &vec, const Vec2 &sc) const {
		return GetNormal().Dot(vec-GetMid(sc));
	}

	/*
	=====================
	Line::IsFacing
	=====================
	*/
	bool Line::IsFacing(const Vec2 &vec, const Vec2 &sc) const {
		return RelativeDot(vec, sc) >= 0.f;
	}




	/*
	=====================
	PolygonShape::PolygonShape
	=====================
	*/
	PolygonShape::PolygonShape(Vec2 vertices[], int vertexCount, const GameNode *par) {
		PimAssert(vertexCount > 2, "Error: A shape must have 3 or more vertices!");
		PimAssert(par != NULL, "Error: A shape must be provided a game node parent!");
		parent = par;

		/* Wind the vertices CCW */
		for (int i=0; i<vertexCount; i++) {
			int i2 = (i+1 < vertexCount) ? (i+1) : (0);
			Vec2 edge = vertices[i2] - vertices[i];

			for (int j=0; j<3; j++) {
				if (j == i || j == i2) {
					continue;
				}

				Vec2 r = vertices[j] - vertices[i];
				if (edge.Cross(r) <= 0.f) {
					Vec2 tmp = vertices[j];
					vertices[j] = vertices[i];
					vertices[i] = tmp;
				}
			}
		}

		int i=0;
		while (i<vertexCount-1) {
			Vec2 *v1 = &vertices[i];
			Vec2 *v2 = &vertices[++i];
			lines.push_back(new Line(*v1, *v2, this));
		}

		lines.push_back(new Line(vertices[i], vertices[0], this));

		// Calculate the normal (some of the line's normals are inverted)
		for (unsigned int i=0; i<lines.size(); i++) {
			Line *cur = lines[i];
			Line *next = (i==lines.size()-1) ? (lines[0]) : (lines[i+1]);
			Line *prev = (i==0) ? (lines[lines.size()-1]) : (lines[i-1]);

			float d = cur->GetNormal().Dot(next->GetP2() - cur->GetP1());
			if (d > 0.f) {
				// Flip the normal
				cur->normal *= Vec2(-1.f, -1.f);
			}
		}
	}

	/*
	=====================
	PolygonShape::~PolygonShape
	=====================
	*/
	PolygonShape::~PolygonShape() {
		for (unsigned int i=0; i<lines.size(); i++) {
			delete lines[i];
		}
	}

	/*
	=====================
	PolygonShape::ShapeContains
	=====================
	*/
	bool PolygonShape::ShapeContains(Vec2 &vec) const {
		for (unsigned int i=0; i<lines.size(); i++) {
			if (lines[i]->RelativeDot(vec) >= 0.f) {
				return false;
			}
		}
		return true;
	}
	
	/*
	=====================
	PolygonShape::GetCenter
	=====================
	*/
	Vec2 PolygonShape::GetCenter() const {
		Vec2 c(0.f, 0.f);

		int i=0;
		for (unsigned i=0; i<lines.size(); i++) {
			c += lines[i]->GetP1();
			c += lines[i]->GetP2();
			i += 2;
		}

		return c / (float)i;
	}

	/*
	=====================
	PolygonShape::GetParent
	=====================
	*/
	const GameNode* PolygonShape::GetParent() const {
		return parent;
	}

	/*
	=====================
	PolygonShape::DebugDraw
	=====================
	*/
	void PolygonShape::DebugDraw() {
		/*
    		The matrix should at this point be translated to the parent's
    		position, so there's no need to use the getter methods on the
    		line's points - they will be drawn relative in both rotation
    		and position of the parent regardless.
		*/
		glDisable(GL_TEXTURE_2D);


		// Render the fill
		glColor4ub(255,0,0,100);
		glBegin(GL_TRIANGLE_FAN);
		for (unsigned int i=0; i<lines.size(); i++) {
			glVertex2f(lines[i]->p1.x, lines[i]->p1.y);
			glVertex2f(lines[i]->p2.x, lines[i]->p2.y);
		}
		glEnd();

		// Render the outline
		glLineWidth(1.f);
		glColor4ub(255,0,0,255);
		glBegin(GL_LINES);
		for (unsigned int i=0; i<lines.size(); i++) {
			// The line itself
			glVertex2f(lines[i]->p1.x, lines[i]->p1.y);
			glVertex2f(lines[i]->p2.x, lines[i]->p2.y);

			/*
			// The normal
			glColor4ub(255,0,0,255);
			Vec2 p = lines[i]->mid + lines[i]->normal*10.f;
			glVertex2f(lines[i]->mid.x, lines[i]->mid.y);
			glVertex2f(p.x, p.y);
			*/
		}
		glEnd();

		glEnable(GL_TEXTURE_2D);
	}

	/*
	=====================
	PolygonShape::ProjectPolygon
	=====================
	*/
	void PolygonShape::ProjectPolygon(const Vec2 axis, float &min, float &max, 
										const Vec2 offset) {
		float dotProduct = axis.Dot(lines[0]->GetP1()+offset);
		min = dotProduct;
		max = dotProduct;

		for (unsigned i=0; i<lines.size(); i++) {
			dotProduct = (lines[i]->GetP1()+offset).Dot(axis);
			if (dotProduct < min) {
				min = dotProduct;
			} else if (dotProduct > max) {
				max = dotProduct;
			}

			dotProduct = (lines[i]->GetP2()+offset).Dot(axis);
			if (dotProduct < min) {
				min = dotProduct;
			} else if (dotProduct > max) {
				max = dotProduct;
			}
		}
	}
}