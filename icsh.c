#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "executeCommand.h"

#define Seperator " \t\r\n"
#define Split_BUFF_SIZE 1000

// define color in shell
#define RED "\033[0;31m"
#define BLUE "\033[0;34m"
#define CYAN "\033[0;36m"
#define GREEN "\033[0;32m"
#define PURPLE "\033[0;35m"
#define WHITE "\033[0;37m"
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
            if ((input != ' ' && input != '\t' && input != '\r') && shouldRead == 0){
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

int main() {
  int status = 1;
  int bangExit = 0;

  char *line;

  char ** history = malloc(sizeof(char *) * 1024);
  int size_his = 0;

  char **args;
  int size = 0;  

  // clear();
  printf("\033[H\033[J");
  printf("========================================\n");
  printf("%s@Copyright: Krittin Nisunarat%s\n",BLUE,RESET);
  printf("%sWelcome to IC Shell Version 0.1.0%s\n", GREEN, RESET);
  printf("%sStarting IC shell prompt%s\n", PURPLE, RESET);
  printf("========================================\n\n");
  sleep(1);

  while (status){
    char pwd[1024];
    getcwd(pwd, sizeof(pwd));
    printf("%sicsh:%s%s%s%s >>> ", WHITE,RESET,BLUE,pwd,RESET);
    /* 1. read input from user
     * 2. split input by space
     * 3. execute command
     * */
    line = read_line();
    if (strcmp(line, "") == 0){
            continue;
    }
    args = split_line(line, &size);
    status = execute(args, history, size, size_his);

    // printf("size: %d\n", size);
    if (strcmp(args[0], "!!") == 0){
        bangExit++;
    }
    else{
        bangExit = 0;
    }
    if (bangExit == 0){
            history = copy_cmd(args, size);
            size_his = size;
    }
    size = 0;
    free(line);
    free(args);
  }
  free(history);
  return 0;
}
