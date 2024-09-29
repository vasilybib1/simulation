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

  // makes the circle arrays
  vec2 positionArray[100];
  float radiusArray[100];
  for(int i = 0; i < 100; i++){
    positionArray[i] = (vec2){
      (float)rand()/RAND_MAX * 2.0f, 
      (float)rand()/RAND_MAX * 2.0f
    };
    radiusArray[i] = 0.05f;
    //printf("%f %f\n", positionArray[i].x, positionArray[i].y);
  }


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
    glClear(GL_COLOR_BUFFER_BIT);
    
    setActiveShader(s);
    
    int resolutionLoc = glGetUniformLocation(s->program, "u_resolution");
    glUniform2f(resolutionLoc, WIDTH, HEIGHT);
    
    int positionArrLoc = glGetUniformLocation(s->program, "u_position_array");
    glUniform2fv(positionArrLoc, 100, (GLfloat *) positionArray);
    
    int radiusArrLoc = glGetUniformLocation(s->program, "u_radius_array");
    glUniform1fv(radiusArrLoc, 100, radiusArray);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  deleteShader(s);
  
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
