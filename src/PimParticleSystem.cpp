#include "PimParticleSystem.h"

namespace Pim {
ParticleSystem::ParticleSystem(void) {
    speed			= 0.f;
    speedVar		= 0.f;
    size			= 0.f;
    sizeVar			= 0.f;
    endSize			= 0.f;
    endSizeVar		= 0.f;
    lifetime		= 0.f;
    lifetimeVar		= 0.f;
    emissionRate	= 0.f;

    count			= 0;
    maxParticles	= 0;

    vaoID			= 0;
    vboID[0]		= 0;
    vboID[1]		= 0;
}
ParticleSystem::ParticleSystem(string pfile) {
    ParticleSystem::ParticleSystem();
    loadFromParticleFile(pfile);
}
ParticleSystem::~ParticleSystem() {

}

void ParticleSystem::loadFromParticleFile(string pfile) {

}

void ParticleSystem::update(float dt) {

}

void ParticleSystem::draw() {

}
void ParticleSystem::batchDraw() {
    draw();
}
}