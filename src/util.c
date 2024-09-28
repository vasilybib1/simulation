#include <stdio.h>
#include <stdlib.h>

#include "util.h"

// function that reads filename and returns the full file as a char* on heap
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
