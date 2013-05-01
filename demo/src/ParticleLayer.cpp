#include "ParticleLayer.h"


/*
==================
ParticleLayer::ParticleLayer
==================
*/
ParticleLayer::ParticleLayer(void)
{
}

/*
==================
ParticleLayer::~ParticleLayer
==================
*/
ParticleLayer::~ParticleLayer(void)
{
}

/*
==================
ParticleLayer::LoadResources
==================
*/
void ParticleLayer::LoadResources() {
	ListenFrame();

	particleSystem = new ParticleSystem("smoke_particle.png");
	particleSystem->position = Vec2(400.f, 300.f);
	AddChild(particleSystem);

	particleSystem->emitAngle = 60.f;
	particleSystem->emitAngleVariance = 60.f;

	particleSystem->speed = 300.f;
	particleSystem->startColor = Color(0.3f, 1.f, 0.3f, 1.f);
	particleSystem->endColor = Color(0.3f, 1.f, 0.3f, 0.f);

	particleSystem->startSize = 30;
	particleSystem->endSize = 20;
	
	particleSystem->lifetime = 2.f;
	particleSystem->maxParticles = 1000;
	particleSystem->emitRate = 450;
}

/*
==================
ParticleLayer::Update
==================
*/
void ParticleLayer::Update(float dt) {
	GameControl::GetSingleton()->LimitFrame(rand()%60);
}
