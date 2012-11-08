#pragma once
#include "PimSpriteBatchNode.h"
#include "PimException.h"
#include "PimInternal.h"

namespace Pim
{
	// Forward declarations
	struct Color;
	class Vec2;

	// The particle struct
	struct Particle
	{
		Vec2 position;

		float timeToLive;

		float rotation;
		float deltaRotation;

		Color color;
		Color deltaColor;
	};


	// The particle system
	class ParticleSystem : public Sprite
	{
	public:
		ParticleSystem();
		ParticleSystem(std::string particleFile);
		~ParticleSystem();
		
		void loadFromParticleFile(std::string particleFile);
		void initVbo();
		void initVao();

		void update(float dt);

	private:
		void addChild(GameNode *n) 
			{ PimAssert(false, "Error: cannot add child to particle system!"); }

	protected:
		void draw();
		void batchDraw();

		Particle	*parts;
		GLuint		*indices;
		GLuint		*vaoID;		// Vertex array 
		GLuint		*vboID[2];	// Vertex buffer, indices 


		float speed;
		float speedVar;

		float size;
		float sizeVar;

		float endSize;
		float endSizeVar;

		float lifetime;
		float lifetimeVar;

		float emissionRate;

		int count;
		int maxParticles;
	};
}