#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#include "util.h"

void generateColor(vec3* colorArr, int size){
  for(int i = 0; i < size; i++){
    float h = (float)(rand() % 360);
    float s = 1.0f; 
    float v = 1.0f; 
    colorArr[i] = hsv_to_rgb(h, s, v);
  }
}

void resizeCallback(GLFWwindow* window, int width, int height){
  glViewport(0, 0, width, height);
}

void errorCallback(int error, const char* desc){ 
  fprintf(stderr, "Error: %s\n", desc);
}

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

char* readFile(char* filename){
  FILE* file;
  file = fopen(filename, "r");
  if(file == NULL) return NULL;

  fseek(file, 0, SEEK_END);
  int length = ftell(file);
  fseek(file, 0, SEEK_SET);
  
  char *string = malloc(sizeof(char) * (length + 1));
  
  char c;
  int i = 0;
  while( (c = fgetc(file)) != EOF){
    string[i] = c;
    i++;
  }
  string[i] = '\0';
  
  fclose(file);
  return string;
}
