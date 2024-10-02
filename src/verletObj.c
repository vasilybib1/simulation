#include "verletObj.h"
#include "util.h"

void verletObj_update(verletObj* obj, float dt){
  vec2 displacement;
  displacement.x = obj->position.x - obj->positionLast.x;
  displacement.y = obj->position.y - obj->positionLast.y;
  obj->positionLast = obj->position;
  obj->position.x = obj->position.x + displacement.x + obj->acceleration.x * (dt * dt);
  obj->position.y = obj->position.y + displacement.y + obj->acceleration.y * (dt * dt);
  obj->acceleration.x = 0.0f;
  obj->acceleration.y = 0.0f;
}

void verletObj_setVel(verletObj* obj, vec2 vel, float dt){
  obj->positionLast.x = obj->position.x - (vel.x * dt);
  obj->positionLast.y = obj->position.y - (vel.y * dt);
}

void verletObj_addVel(verletObj* obj, vec2 vel, float dt){
  obj->positionLast.x -= vel.x * dt;
  obj->positionLast.y -= vel.y * dt;
}

void verletObj_accelerate(verletObj* obj, vec2 a){
  obj->acceleration.x += a.x;
  obj->acceleration.y += a.y;
}

vec2 verletObj_getVelocity(verletObj obj, float dt){
  return (vec2){(obj.position.x - obj.positionLast.x)/dt, (obj.position.y - obj.positionLast.y)/dt};
}
