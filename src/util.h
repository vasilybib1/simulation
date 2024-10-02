#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#ifndef UTIL_H
#define UTIL_H

typedef struct {
  float x;
  float y;
} vec2;

typedef struct {
  float x;
  float y;
  float z;
} vec3;

void generateColor(vec3* colorArr, int size);
void resizeCallback(GLFWwindow* window, int width, int height);
void errorCallback(int error, const char* desc);
vec3 hsv_to_rgb(float h, float s, float v);
char* readFile(char* filename);

#endif
