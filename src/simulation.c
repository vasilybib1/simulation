#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#include "util.h"
#include "shader.h"

#define PI 3.14159265358979323846

typedef struct {
  float x;
  float y;
} vec2;

typedef struct {
  float r;
  float g;
  float b;
} vec3;

typedef struct {
  vec2 position;
  vec2 positionLast;
  vec2 acceleration;
  float radius;
} verletObj;

const int WIDTH = 800;
const int HEIGHT = 800;

const int SUBSTEPS = 8;
const float MAX_RAD = 0.02f;
const float RAD_VARIANCE = 0.01f;
vec2 GRAVITY = {0, -1000.0f};
const int SIZE = 100;
const float PUSH_FORCE = 50000;

int G_fill = 1;
verletObj cir[SIZE];
vec2 posArr[SIZE];
float radArr[SIZE];
vec3 colorArr[SIZE];

vec3 hsv_to_rgb(float h, float s, float v){
  float c = v * s;
  float x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
  float m = v - c;
  float r, g, b;

  if (h >= 0 && h < 60) {
    r = c, g = x, b = 0;
  } else if (h >= 60 && h < 120) {
    r = x, g = c, b = 0;
  } else if (h >= 120 && h < 180) {
    r = 0, g = c, b = x;
  } else if (h >= 180 && h < 240) {
    r = 0, g = x, b = c;
  } else if (h >= 240 && h < 300) {
    r = x, g = 0, b = c;
  } else {
    r = c, g = 0, b = x;
  }

  return (vec3){r,g,b};
}

void generateColor(int size){
  for(int i = 0; i < size; i++){
    float h = (float)(rand() % 360);
    float s = 1.0f; // + (rand() % 10) / 100.0f;
    float v = 1.0f; // + (rand() % 20) / 100.0f;
    colorArr[i] = hsv_to_rgb(h, s, v);
  }
}

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

void sim_applyGravity(verletObj* obj, float dt, int size){
  for(int i = 0; i < size; i++){
    verletObj_accelerate(&obj[i], GRAVITY);
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

void initCirArray(){
  for(int i = 0; i < SIZE; i++){
    cir[i].position = (vec2){1 + 0.1*(float)rand()/RAND_MAX, 1 + 0.1*(float)rand()/RAND_MAX};
    cir[i].positionLast = cir[i].position;
    cir[i].acceleration = (vec2){0.0, 0.0};
    cir[i].radius = (MAX_RAD - RAD_VARIANCE) + RAD_VARIANCE*(float)rand()/RAND_MAX;
  }
}

void setPositionArr(verletObj* obj, int size){
  for(int i = 0; i < size; i++){
    posArr[i] = obj[i].position;
  }
}
void setRadiusArr(verletObj* obj, int size){
  for(int i = 0; i < size; i++){
    radArr[i] = obj[i].radius;
  }
}

void errorCallback(int error, const char* desc){ 
  fprintf(stderr, "Error: %s\n", desc);
}

static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods){
  if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
  if(key == GLFW_KEY_TAB && action == GLFW_PRESS){
    if(G_fill){ G_fill = 0; glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
    else{ G_fill = 1; glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }
  }
  if(key == GLFW_KEY_SPACE && action == GLFW_PRESS){
    initCirArray();
  }
  if(key == GLFW_KEY_E && action == GLFW_PRESS){
    for(int i = 0; i < sizeof(posArr)/sizeof(posArr[0]); i++){
      printf("%f %f | ", posArr[i].x, posArr[i].y);
    }
    printf("\n");
  }
  if(key == GLFW_KEY_R && action == GLFW_PRESS){
    generateColor(SIZE);
  }
  if(key == GLFW_KEY_F && action == GLFW_PRESS){
    GRAVITY.y = GRAVITY.y * -1;
  }
  if(key == GLFW_KEY_W && action == GLFW_REPEAT){
    for(int i = 0; i < SIZE; i++){
      verletObj_accelerate(&cir[i], (vec2){0, PUSH_FORCE});
    }
  }
  if(key == GLFW_KEY_A && action == GLFW_REPEAT){
    for(int i = 0; i < SIZE; i++){
      verletObj_accelerate(&cir[i], (vec2){-PUSH_FORCE, 0});
    }
  }
  if(key == GLFW_KEY_S && action == GLFW_REPEAT){
    for(int i = 0; i < SIZE; i++){
      verletObj_accelerate(&cir[i], (vec2){0, -PUSH_FORCE});
    }
  }
  if(key == GLFW_KEY_D && action == GLFW_REPEAT){
    for(int i = 0; i < SIZE; i++){
      verletObj_accelerate(&cir[i], (vec2){PUSH_FORCE, 0});
    }
  }
}

void resizeCallback(GLFWwindow* window, int width, int height){
  glViewport(0, 0, width, height);
}


int main(){
  glfwSetErrorCallback(errorCallback);
  if(!glfwInit()){ errorCallback(0, "glfw didnt init"); return -1; }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Simulator", NULL, NULL);
  if(!window){ glfwTerminate(); errorCallback(0, "couldnt make window"); return -1; }
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, keyCallBack);
  glfwSetFramebufferSizeCallback(window, resizeCallback);
  glViewport(0, 0, WIDTH, HEIGHT);
  glfwSetWindowAspectRatio(window, WIDTH, HEIGHT);
  
  int frameBufferSizeX, frameBufferSizeY;
  glfwGetFramebufferSize(window, &frameBufferSizeX, &frameBufferSizeY);
  glViewport(0, 0, frameBufferSizeX, frameBufferSizeY);
 
  struct shader* s = createShaderProgram(
      "./shaders/vertexShader.glsl", 
      "./shaders/fragmentMultiCircleWithCenterShader.glsl"
      //"./shaders/fragmentMultiCircleShader.glsl"
      //"./shaders/fragmentCircleShader.glsl"
    );
  
  float vertices[] = {
    -1.0f, -1.0f,
    1.0f, -1.0f, 
    1.0f, 1.0f, 
    -1.0f, 1.0f
  };
  unsigned int indices[] = {
    0, 1, 2,
    0, 2, 3
  };

  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  initCirArray();
  setPositionArr(cir, SIZE);
  setRadiusArr(cir, SIZE);
  generateColor(SIZE);

  // for fps calculation
  double lastTime = glfwGetTime();
  int nbFrames = 0;
  float dt = 0.001f;
  
  while(!glfwWindowShouldClose(window)){
    // calculates ms per frame and prints the output once a second
    double currentTime = glfwGetTime();
    nbFrames++;
    if(currentTime - lastTime >= 1.0){
      printf("%f ms/frame - %d frames\n", 1000.0/(double)nbFrames, nbFrames);
      nbFrames = 0;
      lastTime += 1.0;
    }
    
    // start of render
    glClear(GL_COLOR_BUFFER_BIT);
    
    setPositionArr(cir, SIZE);
    setRadiusArr(cir, SIZE);
    
    setActiveShader(s);
    
    int resolutionLoc = glGetUniformLocation(s->program, "u_resolution");
    glUniform2f(resolutionLoc, WIDTH, HEIGHT);
    
    int positionArrLoc = glGetUniformLocation(s->program, "u_position_array");
    glUniform2fv(positionArrLoc, SIZE, (GLfloat *) &posArr);
    
    int radiusArrLoc = glGetUniformLocation(s->program, "u_radius_array");
    glUniform1fv(radiusArrLoc, SIZE, (GLfloat *) &radArr);
    
    int centerRadiusLoc = glGetUniformLocation(s->program, "u_center_radius");
    glUniform1f(centerRadiusLoc, 0.9f);
    
    int centerPosLoc = glGetUniformLocation(s->program, "u_center_center");
    glUniform2f(centerPosLoc, 1.0f, 1.0f);
    
    int colorLoc = glGetUniformLocation(s->program, "u_color");
    glUniform3fv(colorLoc, SIZE, (GLfloat *) &colorArr);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glfwSwapBuffers(window);
    glfwPollEvents();
    // end of render

    for(int i = 0; i < SUBSTEPS; i++){
      sim_applyGravity(cir, dt/SUBSTEPS, SIZE);
      sim_checkCollisions(cir, dt/SUBSTEPS, SIZE);
      sim_applyCirBorder(cir, (vec2){1.0f, 1.0f}, 0.9f, SIZE);
      sim_updatePosition(cir, dt/SUBSTEPS, SIZE);
    }
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  deleteShader(s);
  
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
