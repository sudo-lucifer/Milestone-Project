#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "readInput.h"
#include "executeCommand.h"
#include "jobsHandlers.h"

#define BUFFERSIZE 1024

#define RED "\033[0;31m"
#define RESET "\e[0m"

/** Reference:
 *      https://stackoverflow.com/questions/3501338/c-read-file-line-by-line
 *
 *  Description:
 *      This file is for script mode when we run shell prompt enumerator with arguments.
 *      It functions like bash in linux terminal. I use the same process as the main file (icsh.c).
 *      The difference is this will read input from file given by user, instead of keystrokes
 *  
 * */

int checkBankSpace(char * line, int size){
        int seeCharacter = 0;
        for (int i = 0; i < size; i++){
                if (line[i] != ' ' && seeCharacter == 0 && line[i] != '\n' && line[i] != '\t' && line[i] != '\r'){
                        seeCharacter = 1;
                }
                if (seeCharacter == 1){
                        return 0;
                }
        }
        return 1;
}

void script(char*  filename){
        FILE *inputFile = fopen(filename, "r");
        if (inputFile == NULL){
                printf("%sFail to Read File: %s%s\n",RED, filename, RESET);
                return;
        }
        int stat = 0;
        char * line = NULL;
        ssize_t read;
        size_t len = 0;

        char ** splitLine;
        int sizeSplit = 0;

        char ** history = malloc(sizeof(char *) * BUFFERSIZE);
        int sizeHis = 0;

        int bangCount = 0;

        init_jobs();

        while((read = getline(&line, &len, inputFile)) != -1){
                if (line[0] =='\n' || checkBankSpace(line, (int) read) == 1){
                        continue;
                }

                // printf("%s\n", line);
                splitLine = split_line(line, &sizeSplit);
                stat = execute(splitLine, history,sizeSplit, sizeHis, 1);
                if (stat != 1){
                        break;
                }
                
                if (strcmp(line, "!!") == 0){
                    bangCount++;
                }
                else{
                    bangCount = 0;
                }

                if(bangCount == 0){
                        history = copy_cmd(splitLine, sizeSplit);
                        sizeHis = sizeSplit;
                }
                sizeSplit = 0;
                free(splitLine);

        }
        free_jobs();

        if (sizeHis != 0){
                free(history);
        }
        fclose(inputFile);
        if (line) free(line);
        if (stat != 1)
            exit(stat);
        exit(0);
}

