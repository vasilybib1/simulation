#include <stdlib.h>
#include <stdio.h>

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "util.h"

// a function to check that shaders were compiled correctly
void shaderCompilerCheck(unsigned int shader){
  int success;
  char log[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if(!success){
    glGetShaderInfoLog(shader, 512, NULL, log);
    fprintf(stderr, "Shader Error: %s\n", log);
  }
}

// a function to check that the shader program was compiled correctly
void shaderProgramCompilerCheck(unsigned int program){
  int success;
  char log[512];
  glGetProgramiv(program, GL_COMPILE_STATUS, &success);
  if(!success){
    glGetProgramInfoLog(program, 512, NULL, log);
    fprintf(stderr, "Program Error: %s\n", log);
  }
}

// creates a shader on heap and inits everything
struct shader* createShaderProgram(char *vertexShader, char *fragmentShader){
  struct shader* s = malloc(sizeof(struct shader));
  s->vertexShader = glCreateShader(GL_VERTEX_SHADER);
  const char* vertexShaderString = readFile(vertexShader);
  glShaderSource(s->vertexShader, 1, &vertexShaderString, NULL);
  glCompileShader(s->vertexShader);
  shaderCompilerCheck(s->vertexShader);

  s->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  const char* fragmentShaderString = readFile(fragmentShader);
  glShaderSource(s->fragmentShader, 1, &fragmentShaderString, NULL);
  glCompileShader(s->fragmentShader);
  shaderCompilerCheck(s->fragmentShader);
  
  free((char*)vertexShaderString);
  free((char*)fragmentShaderString);

  s->program = glCreateProgram();
  glAttachShader(s->program, s->vertexShader);
  glAttachShader(s->program, s->fragmentShader);
  glLinkProgram(s->program);
  shaderProgramCompilerCheck(s->program);
  return s;
}

// makes current shader program active
void setActiveShader(struct shader *s){
  glUseProgram(s->program);
}

// deletes the shader
void deleteShader(struct shader* s){
  glDeleteShader(s->vertexShader);
  glDeleteShader(s->fragmentShader);
  glDeleteProgram(s->program);
  free(s);
}
