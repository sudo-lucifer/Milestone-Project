#include <signal.h>
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

// import all header file in project
#include "executeCommand.h"
#include "readInput.h"
#include "scriptMode.h"
#include "jobsHandlers.h"


// define color in shell
#define RED "\033[0;31m"
#define BLUE "\033[0;34m"
#define CYAN "\033[0;36m"
#define GREEN "\033[0;32m"
#define PURPLE "\033[0;35m"
#define WHITE "\033[0;37m"
#define RESET "\e[0m"

/* Name: 
 *      Krittin Nisunarat 6280782
 * References:
 *      https://www.theurbanpenguin.com/4184-2/
 *      https://danishpraka.sh/2018/01/15/write-a-shell.html
 *      https://www.geeksforgeeks.org/making-linux-shell-c/
 *
 * Description:
 *      The main class for shell prompt enumerator. the main processes are 3 dfferences.
 *      Those process will keep repeating until the user give and exit command.
 *
 * Process:
 *      1. read input from user
 *      2. split the input with space
 *      3. execute the command from user
 * */

void signal_handler(int signum){
        printf("\n\n");
        return;
}

void sigstop_handler(int signum){
        // printf("\n%sCtrl + z is pressed, but nothing is running in forground%s\n", PURPLE,RESET);
        printf("\n\n");
        return;
}


int main(int argc, char * argv[]) {
    // Ctrl + c
    struct sigaction new_action, old_action;

    new_action.sa_handler = signal_handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;

    sigaction(SIGINT, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN){
            sigaction(SIGINT, &new_action,NULL);
    }

    // Ctrl + z
    struct sigaction new_action1, old_action1;

    new_action1.sa_handler = sigstop_handler;
    sigemptyset(&new_action1.sa_mask);
    new_action1.sa_flags = 0;

    sigaction(SIGTSTP, NULL, &old_action1);
    if (old_action1.sa_handler != SIG_IGN){
            sigaction(SIGTSTP, &new_action1,NULL);
    }

    // ignore exit shell signal after sigint and sigstop are activated
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigaction(SIGTTOU, &sa,NULL);

    if (argc > 1){
        for (int i = 1; i < argc; i++){
            printf("%sRunning command lines from: %s%s%s%s\n", BLUE,RESET,GREEN,argv[i], RESET);
            script(argv[i]);
            printf("\n");
        }            
        exit(0);
    }
    
    int status = 1;
    int bangExit = 0;

    char *line;

    char ** history = malloc(sizeof(char *) * 1024);
    int size_his = 0;

    char **args;
    int size = 0;  

    init_jobs();

    // clear();
    printf("\033[H\033[J");
    printf("========================================\n");
    printf("%s@Copyright: Krittin Nisunarat%s\n",BLUE,RESET);
    printf("%sWelcome to IC Shell Version 0.6.0%s\n", GREEN, RESET);
    printf("%sStarting IC shell prompt%s\n", PURPLE, RESET);
    printf("========================================\n\n");
    sleep(1);

    /* 1. read input from user
     * 2. split input by space
     * 3. execute command
     * */
    while (status){
        char buffer[4096];
        char * pwd = getcwd(buffer, sizeof(buffer));
        // printf("%s\n",test);
        printf("%sicsh:%s%s%s%s >>> ", WHITE,RESET,BLUE,pwd,RESET);
        line = read_line();
        if (strcmp(line, "") == 0){
            continue;
        }
        args = split_line(line, &size);
        status = execute(args, history, size, size_his, 0);

        if (status != 1){
                break;
        }

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
    free_jobs();
    return status;
}
