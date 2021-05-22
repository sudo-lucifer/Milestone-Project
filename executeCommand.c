#include <fcntl.h>
#include <stdio.h> 
#include <signal.h>
#include <string.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

// define color in shell
#define CYAN "\033[0;36m"
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define PURPLE "\033[0;35m"
#define RESET "\e[0m"

/* Process:
 *      1. search internal command in the library of our shell
 *      2. if no internal command match, use external command by forking system call
 *      3. if no internal and external command, return 0. OR, there is exit command
 * */
int exitValue = 0;

int checkIORedirect(char ** args, int size, int inOrOut){
        char * sign[2];
        sign[0] = "<";
        sign[1] = ">";

        for (int i = 0; i < size; i++){
                if (strcmp(args[i], sign[inOrOut]) == 0){
                        return i;
                }
        }
        return -1;
}

void childHandler(int sig, siginfo_t * sip, void * notused){
        int status;

        status = 0;
        if (sip->si_pid == waitpid(sip->si_pid, &status, WNOHANG)){
                if (WIFEXITED(status) || WTERMSIG(status)){
                        printf("The child is gone PID: %d\n", sip->si_pid);
                }
        }
}

// copy command before > and < sign
char** getNewCmd(char** args, int index_to){
        char ** cmd = malloc((sizeof(char *) * index_to) + 1);
        for (int i = 0; i < index_to; i++){
                cmd[i] = strdup(args[i]);
        }
        cmd[index_to] = NULL;
        return cmd;
}

// function for execute commands from system
void executeBySyscall(char ** args, int size){
        // instal signal for child process to completely exit (not zombie)
        struct sigaction action;
        action.sa_sigaction = childHandler;
        sigfillset(&action.sa_mask);
        action.sa_flags = SA_SIGINFO;
        sigaction(SIGCHLD, &action, NULL);

        struct sigaction sa;

        pid_t cpid = fork();
        
        if (cpid == 0){
                // unblock signal sigint and sigstop
                sa.sa_handler = SIG_DFL;
                sigaction(SIGINT, &sa,NULL);
                sigaction(SIGTSTP, &sa, NULL);

                // set seperate group for child process in forground
                cpid = getpid();
                setpgid(cpid,cpid);
                tcsetpgrp(0, cpid);

                // int index_inRedirect = checkIORedirect(args, size, 0);
                int index_outRedirect = checkIORedirect(args, size, 1);
                int index_inRedirect = checkIORedirect(args, size, 0);
                int contain_in = 0;
                int contain_out = 0;
                char ** new_args = NULL;

                // case for the input that have < or > but empty in front
                // ex. > filename
                if (!index_inRedirect || !index_outRedirect){
                    printf("%sNo command to excecute%s\n", PURPLE, RESET);
                    exit(EXIT_FAILURE);
                }

                // for invalid command that does not have file name ie. cat filename >, or > only
                if (args[index_outRedirect + 1] == NULL || args[index_inRedirect + 1] == NULL){
                        printf("%sCan not redirect file with empty name%s\n",PURPLE,RESET);
                        exit(EXIT_FAILURE);
                }

                // input redirection
                if (index_inRedirect != -1){
                        int in = open(args[index_inRedirect + 1], O_RDONLY);
                        contain_in = 1;

                        if (in <= 0){
                                printf("%sCan not open file name:%s %s%s%s\n", PURPLE, RESET, GREEN,args[index_inRedirect + 1], RESET);
                                exit(EXIT_FAILURE);
                                
                        }

                        dup2(in, 0);
                        close(in);
                }

                // output redirection
                if (index_outRedirect != -1){
                        int out = open(args[index_outRedirect + 1], O_TRUNC | O_CREAT | O_WRONLY, 0666);
                        contain_out = 1;

                        if (out <= 0){
                                printf("%sCan not open or create file name:%s %s%s%s\n", PURPLE, RESET, GREEN,args[index_outRedirect + 1], RESET);
                                exit(EXIT_FAILURE);
                        }

                        dup2(out,1);
                        close(out);
                }

                // split string before redirection sign exit
                if (contain_in){
                        new_args = getNewCmd(args, index_inRedirect);
                }
                else if (contain_out){
                        new_args = getNewCmd(args, index_outRedirect);
                }
                else if (contain_in && contain_out){
                        if (contain_in < contain_out){
                            new_args = getNewCmd(args, index_inRedirect);
                        }
                        else{
                            new_args = getNewCmd(args, index_outRedirect);
                        }
                }

                // execute by case
                if (contain_in || contain_out){
                        if(execvp(new_args[0], new_args) < 0){
                                printf("%sicsh: Command Not Found: %s%s\n", PURPLE,new_args[0],RESET);
                                free(new_args);
                                exit(EXIT_FAILURE);
                        }
                        else {
                                free(new_args);
                                exit(EXIT_SUCCESS);
                        }
                }
                else {
                        if(execvp(args[0], args) < 0){
                                printf("%sicsh: Command Not Found: %s%s\n", PURPLE,args[0],RESET);
                                exit(EXIT_FAILURE);
                        }
                        else {
                                exit(EXIT_SUCCESS);
                        }
                }
        }
        else if (cpid < 0){
                printf("%sicsh: Error Forking%s\n", RED, RESET);
        }
        else{
                setpgid(cpid,cpid);
                tcsetpgrp(0, cpid);
                int stat;
                waitpid(cpid, &stat,WUNTRACED);
                tcsetpgrp(0, getpid());

                if (WIFEXITED(stat)){
                        exitValue = WEXITSTATUS(stat);
                }
                else if (WIFSIGNALED(stat)){
                        printf("\n");
                        exitValue = WTERMSIG(stat);
                }
                else{
                        printf("\n");
                }
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
                        printf("%sicsh: No Such File or Directory: %s%s%s%s\n", PURPLE, RESET, GREEN, args[1], RESET);
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
    cmd[2] = "clear";
    cmd[3] = "cd";
    cmd[4] = "echo";

    for (int i = 0; i < 5;i++){
            if (strcmp(args[0], cmd[i]) == 0){
                    status = i;
            }
    }    


    if(status == -1){
            executeBySyscall(args, size);
            return 1;
    }

    // save default standard output
    int default_stdout = dup(1);
    int index_outRedirect =checkIORedirect(args, size,1);
    int contain_out = 0;

    // case for the input that have < or > but empty in front
    // ex. > filename
    if (!index_outRedirect){
                printf("%sNo command to excecute%s\n", PURPLE, RESET);
                return 1;
        }
    // for invalid command that does not have file name ie. cat filename >, or > only
    if (args[index_outRedirect + 1] == NULL){
                printf("%sCan not redirect file with empty name%s\n",PURPLE,RESET);
                return 1;
    }

    // redirect for internal command
    if (index_outRedirect != -1){
                int out = open(args[index_outRedirect + 1], O_TRUNC | O_CREAT | O_WRONLY, 0666);
                contain_out = 1;

                if (out <= 0){
                        printf("%sCan not open or create file name:%s %s%s%s\n", PURPLE, RESET, GREEN,args[index_outRedirect + 1], RESET);
                        return 1;
                }

                dup2(out,1);
                close(out);
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
                printf("\033[H\033[J");
                break;
        case 3:
                cd(args,size);
                break;
        case 4:
                if (size != 1){
                        int limit = size;
                        if (contain_out){
                                limit = index_outRedirect;
                        }

                        for (int i = 1; i < limit; i++){
                                if (strcmp(args[i], "$?") == 0){
                                        printf("%d",exitValue);
                                        exitValue = 0;
                                        break;
                                }
                            printf("%s ",args[i]);
                        }
                }
                printf("\n");
                break;
    }
    // set back to default
    dup2(default_stdout, 1);
    return 1;
}

