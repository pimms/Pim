#pragma once

#include "Box2D\Box2D.h"
#include "Pim.h"


Pim::Vec2 toPim(const b2Vec2 v);
b2Vec2 tob2(const Pim::Vec2 v);

void debugDrawShape(b2Body *body, b2Vec2 offset);
