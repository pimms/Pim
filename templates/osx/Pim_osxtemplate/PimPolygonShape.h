#pragma once

#include "PimInternal.h"

namespace Pim {
	class GameNode;
	class Sprite;
	class PolygonShape;
	class Vec2;
	class LightingSystem;
	class CollisionManager;

	class Line {
	private:
		friend class PolygonShape;

	public:
							Line(const Vec2 &v1, const Vec2 &v2, const PolygonShape *p);
		Vec2				GetP1(const Vec2 &scale = Vec2(1,1)) const;
		Vec2				GetP2(const Vec2 &scale = Vec2(1,1)) const;
		Vec2				GetNormal(const Vec2 &scale = Vec2(1,1)) const;
		Vec2				GetNormalEnd(const Vec2 &scale = Vec2(1,1)) const;
		Vec2				GetMid(const Vec2 &scale = Vec2(1,1)) const;
		float				RelativeDot(const Vec2 &vec, const Vec2 &scale = Vec2(1,1)) const;
		bool				IsFacing(const Vec2 &vec, const Vec2 &scale = Vec2(1,1)) const;

	private:
		const PolygonShape	*shape;
		Vec2				p1;
		Vec2				p2;
		Vec2				mid;
		Vec2				normal;

							Line() { shape=NULL; }
							Line(const Line&) { shape=NULL; }
	};


	class PolygonShape {
	private:
		friend class Line;
		friend class GameNode;
		friend class Sprite;
		friend class LightingSystem;
		friend class CollisionManager;

	public:
		vector<Line*>		lines;

							PolygonShape(Vec2 vertices[], int vertCount, const GameNode *parent);
							~PolygonShape();
		bool				ShapeContains(Vec2 &vec) const;
		Vec2				GetCenter() const;
		const GameNode*		GetParent() const;

	private:
		const GameNode		*parent;

							PolygonShape() { parent=NULL; }
							PolygonShape(const PolygonShape&) { parent=NULL; }
		void				ProjectPolygon(const Vec2 axis, float &min, float &max, 
											const Vec2 offset=Vec2(0.f,0.f));
		void				DebugDraw();
	};
}