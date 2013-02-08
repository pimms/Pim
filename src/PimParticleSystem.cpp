#include "PimParticleSystem.h"
#include "PimGameControl.h"
#include "PimVec2.h"
#include "PimHelperFunctions.h"

namespace Pim {

	/*
	==================
	ParticleSystem::Particle::Particle
	==================
	*/
	ParticleSystem::Particle::Particle() {
		rotation		= 0.f;
		size			= 1.f;
		startSize		= 1.f;
		endSize			= 1.f;
		age				= 0.f;
		lifetime		= 2.f;
	}


	/*
	==================
	ParticleSystem::ParticleSystem
	==================
	*/
	ParticleSystem::ParticleSystem(string texturePath)
	: Sprite(texturePath) {
		ListenFrame();

		maxParticles			= 20000;
		emitRate				= 100.f;
		emitAngle				= 45.f;
		emitAngleVariance		= 90.f;
		speed					= 300.f;
		speedVariance			= 0.f;
		startSize				= 90.f;
		startSizeVariance		= 30.f;
		endSize					= 10.f;
		endSizeVariance			= 0.f;
		startColor				= Color(0.4f, 0.4f, 1.f, 1.f);
		endColor				= Color(0.4f, 0.4f, 1.f, 0.f);
		startPosition			= Vec2(0.f, 0.f);
		startPositionVariance	= Vec2(50.f, 0.f);
		gravity					= Vec2(0.f, -1000.f);

		timeSinceLastEmit		= 0.f;
	}

	/*
	==================
	ParticleSystem::ParticleSystem
	==================
	*/
	ParticleSystem::ParticleSystem() {
		ListenFrame();

		maxParticles			= 200;
		emitRate				= 100.f;
		emitAngle				= 90.f;
		emitAngleVariance		= 0.f;
		speed					= 100.f;
		speedVariance			= 100.f;
		startSize				= 40.f;
		startSizeVariance		= 30.f;
		endSize					= 10.f;
		endSizeVariance			= 0.f;
		startColor				= Color(1.f, 0.f, 0.f, 1.f);
		endColor				= Color(1.f, 0.f, 0.f, 1.f);
		startPosition			= Vec2(0.f, 0.f);
		startPositionVariance	= Vec2(50.f, 0.f);
		gravity					= Vec2(0.f, 0.f);

		timeSinceLastEmit		= 0.f;
	}

	/*
	==================
	ParticleSystem::~ParticleSystem
	==================
	*/
	ParticleSystem::~ParticleSystem() {
	}

	/*
	==================
	ParticleSystem::InitiateParticle
	==================
	*/
	void ParticleSystem::InitiateParticle(Particle &particle) {
		particle.velocity = Vec2::UnitDegree(RandF(emitAngle, emitAngle+emitAngleVariance));
		particle.velocity *= RandF(speed, speed+speedVariance);

		particle.position = Vec2::Interpolate(
			startPosition, 
			startPosition + startPositionVariance,
			(float)rand() / (float)RAND_MAX
		);

		particle.size = RandF(startSize, startSize+startSizeVariance);
		particle.endSize = RandF(endSize, endSize+endSizeVariance);
	}

	/*
	==================
	ParticleSystem::UpdateParticle
	==================
	*/
	void ParticleSystem::UpdateParticle(Particle &particle, float dt) {
		particle.age += dt;

		particle.velocity += gravity * dt;
		particle.position += particle.velocity * dt;

		particle.color = Color::Interpolate(
			startColor, 
			endColor, 
			particle.age / particle.lifetime
		);

		// Interpolate the size based on age
		particle.size = startSize - ((startSize-endSize) * (particle.age/particle.lifetime));
	}

	/*
	==================
	ParticleSystem::Update
	==================
	*/
	void ParticleSystem::Update(float dt) {
		timeSinceLastEmit += dt;
		int diff = maxParticles - particles.size();
		
		if (diff > 0) {
			for (int i=0; i<diff && timeSinceLastEmit > (1.f/emitRate); i++) {
				Particle particle;
				InitiateParticle(particle);

				particles.push_back(particle);
				timeSinceLastEmit -= (1.f/emitRate);
			}
		} else if (diff < 0) {
			if (-diff > particles.size()) {
				particles.clear();
			} else {
				particles.erase(particles.end()+diff);
			}
		}

		for (unsigned i=0; i<particles.size(); i++) {
			if (particles[i].age >= particles[i].lifetime) {
				particles.erase(particles.begin() + i--);
			} else {
				UpdateParticle(particles[i], dt);
			}
		}

		vertices.resize(particles.size()*4, Vertex());

		for (unsigned i=0; i<particles.size(); i++) {
			float vertCoordX = particles[i].size / 2.f;
			float vertCoordY = particles[i].size / 2.f;

			unsigned vertexIdx = i*4;
			Vertex &v0	= vertices[vertexIdx + 0];
			Vertex &v1	= vertices[vertexIdx + 1];
			Vertex &v2	= vertices[vertexIdx + 2];
			Vertex &v3	= vertices[vertexIdx + 3];

			v0.position = particles[i].position + Vec2(-vertCoordX, -vertCoordY);
			v0.color    = particles[i].color;
			v0.texCoord = Vec2(0.f, 0.f);

			v1.position = particles[i].position + Vec2( vertCoordX, -vertCoordY);
			v1.color    = particles[i].color;
			v1.texCoord = Vec2(1.f, 0.f);

			v2.position = particles[i].position + Vec2( vertCoordX,  vertCoordY);
			v2.color    = particles[i].color;
			v2.texCoord = Vec2(1.f, 1.f);

			v3.position = particles[i].position + Vec2(-vertCoordX,  vertCoordY);
			v3.color    = particles[i].color;
			v3.texCoord = Vec2(0.f, 1.f);
		}
	}

	/*
	==================
	ParticleSystem::Draw
	==================
	*/
	void ParticleSystem::Draw() {
		glPushMatrix();

		// Update view matrix
		Vec2 fac = GameControl::GetSingleton()->GetCoordinateFactor();

		glTranslatef(position.x / fac.x, position.y / fac.y, 0.f);
		glRotatef(rotation, 0.f, 0.f, 1.f);

		if (particles.size()) {
			glPushMatrix();

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texID);

			fac = GameControl::GetSingleton()->GetWindowScale();
			glScalef(scale.x * fac.x, scale.y * fac.y, 1.f);
			glColor4f(color.r, color.g, color.b, color.a);

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			glVertexPointer  (2, GL_FLOAT, sizeof(Vertex), &vertices[0].position);
			glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &vertices[0].texCoord);
			glColorPointer	 (4, GL_FLOAT, sizeof(Vertex), &vertices[0].color);

			glDrawArrays(GL_QUADS, 0, vertices.size());

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

			glPopMatrix();
		}

		glPopMatrix();
	}

	/*
	==================
	ParticleSystem::BatchDraw
	==================
	*/
	void ParticleSystem::BatchDraw() {
		Draw();
	}
}