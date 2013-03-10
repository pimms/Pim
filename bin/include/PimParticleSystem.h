#pragma once
#include "PimInternal.h"
#include "PimSprite.h"

namespace Pim {
	/**
	 @class 		ParticleSystem
	 @brief 		Particle emitting class.
	 @details 		A single particle system can only emit particles according
	 				to a single configuration. If you want advanced effects, you
	 				should use multiple ParticleSystems and add them to the same
	 				GameNode.
	 
	 				Most attributes in the particle system has a default and a
	 				@e variance variable. The actual value of the particles will
	 				be in the range [base, base+variance].
	 
	 				A visual particle editor is a goal, but at the time not a 
	 				priority.
	 */

	class ParticleSystem : public Sprite {
	protected:
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
		/**
		 @struct 		Particle
		 @brief 		Particle used by ParticleSystem
		 @details 		Only used for storing the result of the particle simulation,
		 				the struct does nothing on it's own.
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

		/**
		 @struct 		Vertex
		 @brief 		Describes how and where a particle will be drawn.
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
