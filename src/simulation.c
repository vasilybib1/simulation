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
  vec2 position;
  vec2 positionLast;
  vec2 acceleration;
  float radius;
} verletObj;

const int WIDTH = 1000;
const int HEIGHT = 1000;

const vec2 GRAVITY = {0, -1000.0f};

int G_fill = 1;
verletObj cir[10];
vec2 posArr[10];
float radArr[10];

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

// TODO: fix the constraints 
void sim_applyBorder(verletObj* obj, float dt, int size){
  for(int i = 0; i < size; i++){
    if(obj[i].position.y - obj[i].radius <= 0.0f){
      verletObj_accelerate(&obj[i], (vec2){0.0f, GRAVITY.y * -1});
      obj[i].position.y = obj[i].radius;
    }
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
    for(int i = 0; i < 10; i++){
      cir[i].position = (vec2){0.1 + (i*0.2), 1 + 0.8*(float)rand()/RAND_MAX};
      cir[i].positionLast = cir[i].position;
      cir[i].acceleration = (vec2){0.0, 0.0};
      cir[i].radius = 0.1 + 0.1 * (float)rand()/RAND_MAX;
    }
    setPositionArr(cir, 10);
    setRadiusArr(cir, 10);
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
      "./shaders/fragmentMultiCircleShader.glsl"
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

  for(int i = 0; i < 10; i++){
    cir[i].position = (vec2){0.1 + (i*0.2), 1 + 0.8*(float)rand()/RAND_MAX};
    cir[i].positionLast = cir[i].position;
    cir[i].acceleration = (vec2){0.0, 0.0};
    cir[i].radius = 0.1 + 0.1 * (float)rand()/RAND_MAX;
  }
  setPositionArr(cir, 10);
  setRadiusArr(cir, 10);

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
    
    setPositionArr(cir, 10);
    setRadiusArr(cir, 10);
    
    setActiveShader(s);
    
    int resolutionLoc = glGetUniformLocation(s->program, "u_resolution");
    glUniform2f(resolutionLoc, WIDTH, HEIGHT);
    
    int positionArrLoc = glGetUniformLocation(s->program, "u_position_array");
    glUniform2fv(positionArrLoc, 10, (GLfloat *) &posArr);
    
    int radiusArrLoc = glGetUniformLocation(s->program, "u_radius_array");
    glUniform1fv(radiusArrLoc, 10, (GLfloat *) &radArr);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glfwSwapBuffers(window);
    glfwPollEvents();
    // end of render
    
    sim_updatePosition(cir, dt, 10);
    sim_applyGravity(cir, dt, 10);
    sim_applyBorder(cir, dt, 10);
    
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  deleteShader(s);
  
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
