#include "util.h"
#include "verletObj.h"

#include <math.h>

void sim_applyGravity(verletObj* obj, vec2 gravity, float dt, int size){
  for(int i = 0; i < size; i++){
    verletObj_accelerate(&obj[i], gravity);
  }
}

void sim_updatePosition(verletObj* obj, float dt, int size){
  for(int i = 0; i < size; i++){
    verletObj_update(&obj[i], dt);
  }
}

void sim_applyCirBorder(verletObj* obj, vec2 center, float radius, int size){
  for(int i = 0; i < size; i++){
    vec2 v = (vec2){center.x - obj[i].position.x, center.y - obj[i].position.y};
    float dist = sqrt(v.x * v.x + v.y * v.y);
    if(dist > (radius - obj[i].radius)){
      vec2 n = (vec2){v.x / dist, v.y / dist};
      obj[i].position.x = center.x - n.x * (radius - obj[i].radius);
      obj[i].position.y = center.y - n.y * (radius - obj[i].radius);
    }
  }
}

void sim_checkCollisions(verletObj* obj, float dt, int size){
  float responseCoef = 0.75f;
  for(int i = 0; i < size; i++){
    for(int j = i+1; j < size; j++){
      vec2 v = (vec2){obj[i].position.x - obj[j].position.x, obj[i].position.y - obj[j].position.y};
      float dist2 = v.x * v.x + v.y * v.y;
      float minDist = obj[i].radius + obj[j].radius;
      if(dist2 < minDist * minDist){
        float dist = sqrt(dist2);
        vec2 n = (vec2){v.x / dist, v.y / dist};
        float massRatio1 = obj[i].radius / (obj[i].radius + obj[j].radius);
        float massRatio2 = obj[j].radius / (obj[i].radius + obj[j].radius);
        float delta = 0.5f * responseCoef *  (dist - minDist);
        obj[i].position.x -= n.x * (massRatio2 * delta);
        obj[i].position.y -= n.y * (massRatio2 * delta);
        obj[j].position.x += n.x * (massRatio1 * delta);
        obj[j].position.y += n.y * (massRatio1 * delta);
      }
    }
  }
}
