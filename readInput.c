#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>

#define Seperator " \t\r\n"
#define Split_BUFF_SIZE 1000

#define RED "\033[0;31m"
#define RESET "\e[0m"

// split the user's input to get ready for execution
char **split_line(char * line, int * size) {
  int buffsize = Split_BUFF_SIZE;
  int position = 0;
  char **tokens = malloc(buffsize * sizeof(char *));

  if (!tokens) {
    fprintf(stderr, "%sicsh: Memory Allocation Error%s\n", RED, RESET);
    exit(EXIT_FAILURE);
  }
  char * token = strtok(line, Seperator);
  while (token != NULL) {
        // expand token buffer
        if (position >= buffsize) {
                buffsize += 500;
                tokens = realloc(tokens, buffsize * sizeof(char * ));

                if (!tokens) {
                        fprintf(stderr, "%sicsh: Memory Allocation Error%s\n", RED, RESET);
                        exit(EXIT_FAILURE);
                }
        }
        tokens[position] = token;
        *size += 1;
        position++;

        token = strtok(NULL, Seperator);
  }
  tokens[position] = NULL;

  return tokens;
}



char *read_line() {
  int shouldRead = 0;
  int buffsize = 1024;
  int position = 0;
  char *buffer = malloc(sizeof(char) * buffsize);

  if (!buffer) {
    fprintf(stderr, "%sicsh: Memory Allocation Error%s\n", RED, RESET);
    exit(EXIT_FAILURE);
  }

  while (1) {
    int input = getchar();
    // end of user's input
    // printf("%d\n",shouldRead);
    if (input == EOF || input == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
            if ((input != ' ' && input != '\t' && input != '\r')  && shouldRead == 0){
                shouldRead = 1;
            }
            if (shouldRead == 1){
                buffer[position] = input;
                position++;
            }
    }

    // buffer is full (include the size of buffer)
    if (position >= buffsize) {        
        buffsize += 512;
        buffer = realloc(buffer, buffsize * sizeof(char));

        if (!buffer) {
            fprintf(stderr, "%sicsh: Failure Reallocating Buffersize%s\n", RED, RESET);
            exit(EXIT_FAILURE);
        }
    }
  }
}

// copy current command for doubl bang
char** copy_cmd(char** args, int size){
        int buffersize = 1024;
        char ** buffer = malloc(sizeof(char *) * buffersize);

        for (int i = 0; i < size; i++){
                if (i >=buffersize){
                    buffersize += 512;
                    buffer = realloc(buffer, buffersize);
                }
                if (!buffer){
                    fprintf(stderr, "%sicsh: Failure Reallocating Buffersize%s\n", RED, RESET);
                    exit(EXIT_FAILURE);
                }
                buffer[i] = strdup(args[i]);
        }
        return buffer;
}
