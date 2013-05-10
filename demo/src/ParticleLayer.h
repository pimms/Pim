#pragma once

#ifdef WIN32
#include "Pim.h"
#elif defined(__APPLE__)
#include <Pim/Pim.h>
#endif


using namespace Pim;

class ParticleLayer : public Layer
{
public:
								ParticleLayer(void);
								~ParticleLayer(void);
	void						LoadResources();
	void						Update(float dt);

	ParticleSystem				*particleSystem;
};

