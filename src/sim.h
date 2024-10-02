#include "util.h"
#include "verletObj.h"

#ifndef SIM_H
#define SIM_H

void sim_applyGravity(verletObj* obj, vec2 gravity, float dt, int size);
void sim_updatePosition(verletObj* obj, float dt, int size);
void sim_applyCirBorder(verletObj* obj, vec2 center, float radius, int size);
void sim_checkCollisions(verletObj* obj, float dt, int size);

#endif
