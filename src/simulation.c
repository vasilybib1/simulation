#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#include "util.h"
#include "shader.h"
#include "sim.h"
#include "verletObj.h"

#define PI 3.14159265358979323846

const int WIDTH = 800;
const int HEIGHT = 800;

const int SUBSTEPS = 8;
const float MAX_RAD = 0.02f;
const float RAD_VARIANCE = 0.01f;
const int SIZE = 100;
const float dt = 0.001f;
const float PUSH_FORCE = 50000;

vec2 GRAVITY = {0, -1000.0f};

verletObj cir[SIZE];
vec2 posArr[SIZE];
float radArr[SIZE];
vec3 colorArr[SIZE];

void generateColor(int size){
  for(int i = 0; i < size; i++){
    float h = (float)(rand() % 360);
    float s = 1.0f; // + (rand() % 10) / 100.0f;
    float v = 1.0f; // + (rand() % 20) / 100.0f;
    colorArr[i] = hsv_to_rgb(h, s, v);
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

unsigned int generateTexture(int size, float* data){
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_1D, texture);

  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, size, 0, GL_RED, GL_FLOAT, data);

  glBindTexture(GL_TEXTURE_1D, 0);
  return texture;
}

static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods){
  if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
  if(key == GLFW_KEY_SPACE && action == GLFW_PRESS){
    initCirArray();
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
      "./shaders/vertexTextureShader.glsl", 
      "./shaders/fragmentMultiCircleWithCenterTextureShader.glsl"
      //"./shaders/fragmentMultiCircleWithCenterShader.glsl"
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

  float newRadiusArr[100];
  for(int i = 0; i < 100; i++){
    newRadiusArr[i] = 0.01f + 0.01f*i;
  }
  int tex = generateTexture(SIZE, radArr);





  double lastTime = glfwGetTime();
  int nbFrames = 0;

  while(!glfwWindowShouldClose(window)){
    double currentTime = glfwGetTime();
    nbFrames++;
    if(currentTime - lastTime >= 1.0){
      printf("%f ms/frame - %d frames\n", 1000.0/(double)nbFrames, nbFrames);
      nbFrames = 0;
      lastTime += 1.0;
    }
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    setPositionArr(cir, SIZE);
    setRadiusArr(cir, SIZE);
    
    setActiveShader(s);
    
    int resolutionLoc = glGetUniformLocation(s->program, "u_resolution");
    glUniform2f(resolutionLoc, WIDTH, HEIGHT);
    
    int positionArrLoc = glGetUniformLocation(s->program, "u_position_array");
    glUniform2fv(positionArrLoc, SIZE, (GLfloat *) &posArr);
    
    //int radiusArrLoc = glGetUniformLocation(s->program, "u_radius_array");
    //glUniform1fv(radiusArrLoc, SIZE, (GLfloat *) &radArr);

    glActiveTexture(GL_TEXTURE0 + tex);
    glBindTexture(GL_TEXTURE_1D, tex);
    int textureLoc = glGetUniformLocation(s->program, "radiusTexture");
    glUniform1i(textureLoc, tex);
    
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

    for(int i = 0; i < SUBSTEPS; i++){
      sim_applyGravity(cir, GRAVITY, dt/SUBSTEPS, SIZE);
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
