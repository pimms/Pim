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
	 				be in the range [base, base ± variance].
	 
	 				The ParticleSystem class is a Sprite derivative, and the 
	 				texture loaded by Sprite is used as the particle texture.
	 
	 				A visual particle editor is a goal, but at the time not a 
	 				priority.
	 */

	class ParticleSystem : public Sprite {
	protected:
		struct Particle;
		struct Vertex;

	public:
		enum PositionType {
			PART_RELATIVE,	// The particles will move WITH the system. Default.
			PART_ABSOLUTE	// The particles will move relatively to the point of
							// their emission.
		};
		
		PositionType 			positionType;
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
		float					startRotation;
		float					startRotationVariance;
		float					endRotation;
		float					endRotationVariance;
		float 					lifetime;
		float 					lifetimeVariance;
		Color					startColor;
		Color					startColorVariance;
		Color					endColor;
		Color					endColorVariance;
		Vec2					startPosition;
		Vec2					startPositionVariance;
		Vec2					gravity;

		static float			RanBaseVar(float base, float variance, bool neg=true);
		static float			Interpolate(float start, float end, float fac);

								ParticleSystem(string texturePath);
								ParticleSystem();
								~ParticleSystem(void);
		virtual void			Update(float dt);
		virtual void			Draw();
		virtual void			BatchDraw();
		int						GetParticleCount();
		void					RemoveAllParticles();

	protected:
		/**
		 @struct 		Particle
		 @brief 		Particle used by ParticleSystem
		 @details 		Only used for storing the result of the particle simulation,
		 				the struct does nothing on it's own.
		 */
		struct Particle {
			Vec2 				spawnPos;	// Used with positionType ABSOLUTE
			Vec2				position;
			Vec2				velocity;
			float				startRotation;
			float				endRotation;
			float				rotation;
			Color				color;
			Color				startColor;
			Color				endColor;
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
		 @todo			Rotation
		 */
		struct Vertex {
			Vec2				position;
			Vec2				texCoord;
			Color				color;
		};

		vector<Particle*>		particles;
		vector<Vertex>			vertices;
		float					timeSinceLastEmit;

		void					UpdateAllParticles(float dt);
		void					CreateVertexData();

		virtual void			InitiateParticle(Particle *particle);
		virtual void			UpdateParticle(Particle *particle, float dt);
	};
}
