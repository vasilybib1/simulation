#include <stdio.h>

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

const int WIDTH = 600;
const int HEIGHT = 600;
const int FPS = 60;

void errorCallback(int error, const char* desc){ 
  fprintf(stderr, "Error: %s\n", desc);
}

static void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods){
  if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
    glfwSetWindowShouldClose(window, GLFW_TRUE);
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
    
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
}
