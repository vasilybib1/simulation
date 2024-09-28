#ifndef SHADER_H
#define SHADER_H

struct shader;
struct shader* createShaderProgram(char* vertexShader, char* fragmentShader);
void deleteShader(struct shader* s);
void setActiveShader(struct shader* s);

#endif
