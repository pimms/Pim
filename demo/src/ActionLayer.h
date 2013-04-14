#pragma once

#ifdef WIN32
#include "Pim.h"
#elif defined(__APPLE__)
#include <Pim/Pim.h>
#endif

class ActionLayer : public Pim::Layer
{
public:
					ActionLayer(void);
					~ActionLayer(void);
	void			LoadResources();
	void			Update(float);
	void			ActionCompleted(Pim::BaseAction *a);

private:
	float			time;
};

