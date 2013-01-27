#pragma once

/*

// THE INTEGRATED COLLISION DETECTION LIBRARY IS DEPRECATED.
// THE INTEGRATED COLLISION DETECTION LIBRARY IS DEPRECATED.
// THE INTEGRATED COLLISION DETECTION LIBRARY IS DEPRECATED.

namespace Pim
{
	// forward declarations
	class GameControl;
	class GameNode;
	class Vec2;

	class CollisionManager
	{
	public:
		CollisionManager(void);
		~CollisionManager(void);

		static CollisionManager* getSingleton();

		static Vec2 validateMovement(GameNode*, Vec2 &oldPos, Vec2 &newPos);

		// The two shapes must share the same parent-layer.
		static bool shapesIntersect(GameNode *nodeA, GameNode *nodeB);

	private:
		friend class GameControl;

		static void instantiateSingleton();
		static void clearSingleton();

		Vec2 valMove(GameNode*, Vec2 &oldPos, Vec2 &newPos);


		bool intersect(GameNode*, GameNode*);
		float intervalDistance(float minA, float maxA, float minB, float maxB);

		static CollisionManager *singleton;
	};
}
*/