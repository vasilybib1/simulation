#include <alloca.h>
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

const int WIDTH = 500;
const int HEIGHT = 500;

const vec2 CENTER = (vec2){1.0f, 1.0f};
const float RADIUS = 0.9f;

const float MAX_RAD = 0.02f;
const float RAD_VARIANCE = 0.01f;
const int SIZE = 500;

const int SUBSTEPS = 8;
const float dt = 0.001f;

const float PUSH_FORCE = 50000;
vec2 GRAVITY = {0, -1000.0f};

verletObj obj[SIZE];

int dataTexture;
int colorTexture;

void initObjArray(verletObj* obj, int size){
  for(int i = 0; i < size; i++){
    obj[i].position = (vec2){1 + 0.1*(float)rand()/RAND_MAX, 1 + 0.1*(float)rand()/RAND_MAX};
    obj[i].positionLast = obj[i].position;
    obj[i].acceleration = (vec2){0.0, 0.0};
    obj[i].radius = (MAX_RAD - RAD_VARIANCE) + RAD_VARIANCE*(float)rand()/RAND_MAX;
  }
}

unsigned int generateColorTexture(int size){
  vec3 data[size];
  generateColor(data, size);

  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_1D, texture);

  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, size, 0, GL_RGB, GL_FLOAT, data);

  glBindTexture(GL_TEXTURE_1D, 0);
  return texture;
}

void updateColorTexture(int size, int texture){
  vec3 data[size];
  generateColor(data, size);

  glBindTexture(GL_TEXTURE_1D, texture);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, size, 0, GL_RGB, GL_FLOAT, data);
  glBindTexture(GL_TEXTURE_1D, 0);
}

unsigned int generateDataTexture(verletObj* obj, int size){
  vec3 data[size];
  for(int i = 0; i < size; i++){
    data[i].x = obj[i].position.x;
    data[i].y = obj[i].position.y;
    data[i].z = obj[i].radius;
  }

  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_1D, texture);

  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, size, 0, GL_RGB, GL_FLOAT, data);

  glBindTexture(GL_TEXTURE_1D, 0);
  return texture;
}

void updateDataTexture(verletObj* obj, int size, int texture){
  vec3 data[size];
  for(int i = 0; i < size; i++){
    data[i].x = obj[i].position.x;
    data[i].y = obj[i].position.y;
    data[i].z = obj[i].radius;
  }

  glBindTexture(GL_TEXTURE_1D, texture);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, size, 0, GL_RGB, GL_FLOAT, data);
  glBindTexture(GL_TEXTURE_1D, 0);
}

static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods){
  if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
  if(key == GLFW_KEY_SPACE && action == GLFW_PRESS){
    initObjArray(obj, SIZE);
  }
  if(key == GLFW_KEY_F && action == GLFW_PRESS){
    GRAVITY.y = GRAVITY.y * -1;
  }
  if(key == GLFW_KEY_W && action == GLFW_REPEAT){
    for(int i = 0; i < SIZE; i++){
      verletObj_accelerate(&obj[i], (vec2){0, PUSH_FORCE});
    }
  }
  if(key == GLFW_KEY_A && action == GLFW_REPEAT){
    for(int i = 0; i < SIZE; i++){
      verletObj_accelerate(&obj[i], (vec2){-PUSH_FORCE, 0});
    }
  }
  if(key == GLFW_KEY_S && action == GLFW_REPEAT){
    for(int i = 0; i < SIZE; i++){
      verletObj_accelerate(&obj[i], (vec2){0, -PUSH_FORCE});
    }
  }
  if(key == GLFW_KEY_D && action == GLFW_REPEAT){
    for(int i = 0; i < SIZE; i++){
      verletObj_accelerate(&obj[i], (vec2){PUSH_FORCE, 0});
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

  initObjArray(obj, SIZE);
  colorTexture = generateColorTexture(SIZE);
  dataTexture = generateDataTexture(obj, SIZE);
  colorTexture = generateColorTexture(SIZE);

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
    
    setActiveShader(s);
    
    glUniform1i(glGetUniformLocation(s->program, "u_size"), SIZE);
    glUniform2f(glGetUniformLocation(s->program, "u_resolution"), WIDTH, HEIGHT);
    glUniform3f(glGetUniformLocation(s->program, "u_background"), CENTER.x, CENTER.y, RADIUS);
    
    glActiveTexture(GL_TEXTURE0 + dataTexture);
    glBindTexture(GL_TEXTURE_1D, dataTexture);
    glUniform1i(glGetUniformLocation(s->program, "u_dataTexture"), dataTexture);
    
    glActiveTexture(GL_TEXTURE0 + colorTexture);
    glBindTexture(GL_TEXTURE_1D, colorTexture);
    glUniform1i(glGetUniformLocation(s->program, "u_colorTexture"), colorTexture);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glfwSwapBuffers(window);
    glfwPollEvents();
    
    for(int i = 0; i < SUBSTEPS; i++){
      sim_applyGravity(obj, GRAVITY, dt/SUBSTEPS, SIZE);
      sim_checkCollisions(obj, dt/SUBSTEPS, SIZE);
      sim_applyCirBorder(obj, (vec2){1.0f, 1.0f}, 0.9f, SIZE);
      sim_updatePosition(obj, dt/SUBSTEPS, SIZE);
    }
    
    updateDataTexture(obj, SIZE, dataTexture);
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  deleteShader(s);
  
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
