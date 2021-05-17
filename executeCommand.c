#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// define color in shell
#define CYAN "\033[0;36m"
#define PURPLE "\033[0;35m"
#define RESET "\e[0m"

int execute(char ** args, char ** history, int size, int size_his, int isScriptMode){
    int status = -1;
    char * cmd[4];

    cmd[0] = "exit";
    cmd[1] = "!!";
    cmd[2] = "echo";
    cmd[3] = "clear";

    for (int i = 0; i < 4;i++){
            if (strcmp(args[0], cmd[i]) == 0){
                    status = i;
            }
    }    

    switch (status) {
        case 0:
                if (isScriptMode == 0){
                    printf("%sGood to see you!%s\n",CYAN,RESET);
                }
                // if (strcmp(args[1], ))
                if (size != 1){
                    exit(atoi(args[1]));
                }
                exit(0);
        case 1:
                if (isScriptMode == 0){
                    if (size_his != 0){
                        for (int i = 0; i<size_his; i++){
                                printf("%s ", history[i]);
                        }
                        printf("\n");
                        execute(history, args, size_his,size, isScriptMode);
                    }
                    else { printf("%sicsh: No Command History%s\n", PURPLE,RESET); }
                }
                else{
                    if (size_his != 0){
                        execute(history, args, size_his,size, isScriptMode);
                    }
                    else { printf("%sicsh: No Command History%s\n", PURPLE,RESET); }
                }
                break;
        case 2:
                if (size != 1){
                        for (int i = 1; i < size; i++){
                            printf("%s ",args[i]);
                        }
                }
                printf("\n");
                break;
        case 3:
                printf("\033[H\033[J");
                break;
        default:
                printf("%sicsh: Command Not Found: %s%s\n", PURPLE,args[0],RESET);
    }
    return 1;
}

