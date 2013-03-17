#pragma once
#include "Pim.h"

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

