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
	particleSystem = new ParticleSystem("smoke_particle.png");
	particleSystem->position = Vec2(400.f, 300.f);
	AddChild(particleSystem);
}

