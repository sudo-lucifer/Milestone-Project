#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

// define color in shell
#define CYAN "\033[0;36m"
#define RED "\033[0;31m"
#define PURPLE "\033[0;35m"
#define RESET "\e[0m"

/* Process:
 *      1. search internal command in the library of our shell
 *      2. if no internal command match, use external command by forking system call
 *      3. if no internal and external command, return 0. OR, there is exit command
 * */

// function for execute commands from system
void executeBySyscall(char ** args){
        int status;
        pid_t cpid = fork();
        
        if (cpid == 0){
                if(execvp(args[0], args) < 0){
                        printf("%sicsh: Command Not Found: %s%s\n", PURPLE,args[0],RESET);
                        exit(EXIT_FAILURE);
                }
                exit(EXIT_SUCCESS);
        }
        else if (cpid < 0){
                printf("%sicsh: Error Forking%s\n", RED, RESET);
        }
        else{
                waitpid(cpid, &status,WUNTRACED);
        }
}

void cd(char ** args, int size){
        if (size > 2){
                printf("%sicsh: Too Many Arguments%s\n",PURPLE,RESET);
        }
        else if (size == 2){
                char current[1024];
                getcwd(current,sizeof(current));
                strcat(current,"/");
                strcat(current, args[1]);

                // check if directory existed or it existed but it ir a file
                struct stat s;
                int check = stat(current, &s);
                if (check == -1){
                        printf("%sicsh: No Such File or Directory: %s%s%s%s\n", PURPLE, RESET, CYAN, args[1], RESET);
                }
                else{
                        if (S_ISDIR(s.st_mode)){
                            chdir(args[1]);
                        }
                        else {
                            printf("%sicsh: exist but it is not directory%s\n", PURPLE,RESET);
                        }
                }
        }
        else{
                chdir("/home/u6280782/");
        }


}

// execute internal commands
int execute(char ** args, char ** history, int size, int size_his, int isScriptMode){
    int status = -1;
    char * cmd[5];

    cmd[0] = "exit";
    cmd[1] = "!!";
    cmd[2] = "echo";
    cmd[3] = "clear";
    cmd[4] = "cd";

    for (int i = 0; i < 5;i++){
            if (strcmp(args[0], cmd[i]) == 0){
                    status = i;
            }
    }    


    if(status == -1){
            executeBySyscall(args);
            return 1;
    }

    switch (status) {
        case 0:
                if (isScriptMode == 0){
                    printf("%sGood to see you!%s\n",CYAN,RESET);
                }
                if (size != 1){
                        exit(atoi(args[1]));
                }
                else exit(0);
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
        case 4:
                cd(args,size);
                break;
        // default:
                // printf("%sicsh: Command Not Found: %s%s\n", PURPLE,args[0],RESET);
    }
    return 1;
}

