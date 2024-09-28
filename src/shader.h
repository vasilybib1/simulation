#ifndef SHADER_H
#define SHADER_H

typedef struct shader {
  unsigned int vertexShader;
  unsigned int fragmentShader;
  unsigned int program;
} shader;

struct shader* createShaderProgram(char* vertexShader, char* fragmentShader);
void deleteShader(struct shader* s);
void setActiveShader(struct shader* s);

#endif
