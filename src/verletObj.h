#include "util.h"

#ifndef VERLETOBJ_H
#define VERLETOBJ_H

typedef struct {
  vec2 position;
  vec2 positionLast;
  vec2 acceleration;
  float radius;
} verletObj;

void verletObj_update(verletObj* obj, float dt);
void verletObj_setVel(verletObj* obj, vec2 vel, float dt);
void verletObj_addVel(verletObj* obj, vec2 vel, float dt);
void verletObj_accelerate(verletObj* obj, vec2 a);
vec2 verletObj_getVelocity(verletObj obj, float dt);

#endif
