#pragma once
#include "PimLayer.h"
#include "PimParticleSystem.h"

using namespace Pim;

class ParticleLayer : public Layer
{
public:
								ParticleLayer(void);
								~ParticleLayer(void);
	void						LoadResources();

	ParticleSystem				*particleSystem;
};

