#pragma once
#include "PimInternal.h"
#include "PimSprite.h"

namespace Pim {

	class ParticleSystem : public Sprite
	{
	protected:
		/* Forward declarations */
		struct Particle;
		struct Vertex;

	public:
		int						maxParticles;
		float					emitRate;
		float					emitAngle;
		float					emitAngleVariance;
		float					speed;
		float					speedVariance;
		float					startSize;	
		float					startSizeVariance;
		float					endSize;
		float					endSizeVariance;
		Color					startColor;
		Color					endColor;
		Vec2					startPosition;
		Vec2					startPositionVariance;
		Vec2					gravity;

								ParticleSystem(string texturePath);
								ParticleSystem();
								~ParticleSystem(void);
		virtual void			Update(float dt);
		virtual void			Draw();
		virtual void			BatchDraw();

	protected:
		/*
		==================
		ParticleSystem::Particle

		Represents a particle somewhere along the 2D plane. The structure is 
		unaware of it's texture and state-to-be.
		==================
		*/
		struct Particle {
			Vec2				position;
			Vec2				velocity;
			float				rotation;
			Color				color;
			float				size;
			float				startSize;
			float				endSize;
			float				age;
			float				lifetime;

								Particle();
		};

		/*
		==================
		ParticleSystem::Vertex

		One Vertex is stored for each particle. It contains the position, tex coord
		and color of each particle, neatly stacked into a single buffer.
		==================
		*/
		struct Vertex {
			Vec2				position;
			Vec2				texCoord;
			Color				color;
		};

		vector<Particle>		particles;
		vector<Vertex>			vertices;
		float					timeSinceLastEmit;

		virtual void			InitiateParticle(Particle &particle);
		virtual void			UpdateParticle(Particle &particle, float dt);
	};
}
