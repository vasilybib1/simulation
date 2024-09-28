#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#include "util.h"
#include "shader.h"

#define PI 3.14159265358979323846

const int WIDTH = 600;
const int HEIGHT = 600;
const int FPS = 60;

int G_fill = 1;

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
}

void resizeCallback(GLFWwindow* window, int width, int height){
  glViewport(0, 0, width, height);
}


void generateCircleVerticies(float* vert, float x, float y, float radius, int segments){
  float theta;
  for(int i = 0; i <= segments; i++){
    theta = 2.0f * PI * i / segments;
    vert[2*i] = x + radius * cosf(theta); 
    vert[2*i+1] = y + radius * sinf(theta); 
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
      "./shaders/vertexShader.glsl", 
      "./shaders/fragmentShader.glsl"
    );

  int segments = 100; 
  float radius = 0.5f;
  float* vert = (float*)malloc((segments + 2) * 2 * sizeof(float));
  
  generateCircleVerticies(vert, 0.0f, 0.0f, radius, segments);

  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, (segments + 2) * 2 * sizeof(float), vert, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // for fps calculation
  double lastTime = glfwGetTime();
  int nbFrames = 0;

  while(!glfwWindowShouldClose(window)){
    // calculates ms per frame and prints the output once a second
    double currentTime = glfwGetTime();
    nbFrames++;
    if(currentTime - lastTime >= 1.0){
      printf("%f ms/frame - %d frames\n", 1000.0/(double)nbFrames, nbFrames);
      nbFrames = 0;
      lastTime += 1.0;
    }
    
    // start of loop
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    setActiveShader(s);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments+2);
    
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  free(vert);
  deleteShader(s);
  
  glfwDestroyWindow(window);
  glfwTerminate();
}
