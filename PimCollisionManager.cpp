#include "Stdafx.h"
#include "PimCollisionManager.h"
#include "PimGameControl.h"
#include "PimException.h"
#include "PimVec2.h"
#include "PimGameNode.h"
#include "PimLayer.h"
#include "PimPolygonShape.h"

namespace Pim
{
	CollisionManager* CollisionManager::singleton = NULL;


	CollisionManager::CollisionManager(void)
	{
		PimAssert(!singleton, "Error: multiple collision manager singletons!");
	}
	CollisionManager::~CollisionManager(void)
	{

	}

	void CollisionManager::instantiateSingleton()
	{
		PimAssert(!singleton, "Error: Collision manager already instantiated!");

		singleton = new CollisionManager;
	}
	void CollisionManager::clearSingleton()
	{
		PimAssert(singleton, "Error: Collision manager already deleted!");

		delete singleton;
		singleton = NULL;
	}
	CollisionManager* CollisionManager::getSingleton()
	{
		PimAssert(singleton, "Error: collision manager singleton is null!");
		return singleton;
	}

	Vec2 CollisionManager::validateMovement(GameNode *node, Vec2 &o, Vec2 &n)
	{
		return getSingleton()->valMove(node, o, n);
	}
	Vec2 CollisionManager::valMove(GameNode *moveNode, Vec2 &o, Vec2 &n)
	{
		Layer *l = moveNode->getParentLayer();
		if (!l) return o;

		unsigned short cntA = moveNode->colShape->lines.size();
		
		Vec2 d = n-o;

		bool inter = true;
		bool willInter = true;
		float minIntDist = 4000000.f;
		Vec2 translationAxis;

		for each (GameNode *node in moveNode->getParentLayer()->collisionNodes)
		{
			unsigned short cntB = node->colShape->lines.size();
			

			for (int i=0; i<cntA+cntB; i++)
			{
				Line *line;
				Vec2 axis;
				if (i < cntA)
				{
					line = moveNode->colShape->lines[i];
					axis = line->getNormal();
				}
				else
				{
					line = node->colShape->lines[i-cntA];
					axis = line->getNormal();
				}

				float minA=0, maxA=0, minB=0, maxB=0;
				moveNode->colShape->projectPolygon(axis, minA, maxA,d);
				node->colShape->projectPolygon(axis, minB, maxB);

				if (intervalDistance(minA,maxA,minB,maxB) >= 0.f)
				{
					inter = false;
				}

				if (!willInter || i < cntA) continue;

				float velProjection = axis.dot(d);

				if (velProjection < 0)
					minA += velProjection;
				else
					maxA += velProjection;

				float intDist = intervalDistance(minA,maxA,minB,maxB);
				if (intDist > 0) 
				{
					willInter = false;
					continue;
				}

				intDist = abs(intDist);
				if (intDist < minIntDist)
				{
					minIntDist = intDist;
					translationAxis = axis;

					Vec2 dist = moveNode->colShape->center() - node->colShape->center();
					if (d.dot(translationAxis) < 0)
						translationAxis *= -1.f;
				}
			}
		}

		if (inter)
			return o;

		if (willInter)
			return o + (translationAxis * minIntDist);

		return n;
	}

	bool CollisionManager::shapesIntersect(GameNode *na, GameNode *nb)
	{
		return singleton->intersect(na,nb);
	}

	bool CollisionManager::intersect(GameNode *na, GameNode *nb)
	{
#ifdef _DEBUG
		PimAssert(na->getParentLayer() == nb->getParentLayer(), 
			"Error: only siblings can intersect.");
#endif

		PolygonShape *shA = na->colShape;
		PolygonShape *shB = nb->colShape;

		PimAssert(shA && shB, "Error: nil shape");

		unsigned short cntA = shA->lines.size();
		unsigned short cntB = shB->lines.size();

		for (int i=0; i<cntA+cntB; i++)
		{
			Line *line;
			if (i < cntA)
				line = shA->lines[i];
			else
				line = shB->lines[i-cntA];

			Vec2 axis = line->getNormal();

			float minA=0, maxA=0, minB=0, maxB=0;
			shA->projectPolygon(axis, minA, maxA);
			shB->projectPolygon(axis, minB, maxB);

			if (intervalDistance(minA,maxA,minB,maxB) >= 0.f)
				return false;
		}

		return true;
	}
	float CollisionManager::intervalDistance(float minA, float maxA, float minB, float maxB)
	{
		if (minA < minB) {
			return minB - maxA;
		} else {
			return minA - maxB;
		}
	}
} 