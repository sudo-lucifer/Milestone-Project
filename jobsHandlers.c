#include <fcntl.h>
#include <stdio.h> 
#include <signal.h>
#include <string.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>


#include "jobsHandlers.h"


#define JOBSIZE 100
#define CYAN "\033[0;36m"
#define WHITE "\033[0;37m"
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define PURPLE "\033[0;35m"
#define RESET "\e[0m"


struct process jobs[JOBSIZE];
int runningprocess = 0;

void init_jobs(){
        for (int i = 0; i < JOBSIZE; i++){
                jobs[i].isRunning = -1;
                jobs[i].name = NULL;
                jobs[i].size = -1;
                jobs[i].job_id = -1;
                jobs[i].process_id = -1;
        }
}

void free_jobs(){
        for (int i = 0; i < JOBSIZE; i++){
                if (jobs[i].isRunning && jobs[i].job_id != -1){
                        kill(jobs[i].process_id,SIGINT);
                }
                free(jobs[i].name);
        }
}

int getProcessID(int job_id){
        return jobs[job_id].process_id;
}

int isRunning(int process_id){
        for (int i = 0; i < JOBSIZE; i++){
                if (jobs[i].process_id == process_id){
                        if (jobs[i].isRunning){
                                return 1;
                        }
                }
        }
        return 0;
}

int contain_jobs(int process_id){
        for (int i = 0; i < JOBSIZE; i++){
                if (jobs[i].process_id == process_id){
                        return 1;
                }
        }
        return 0;
}


void remove_jobs(int pid){
        for (int i = 0; i < JOBSIZE; i++){
                if (jobs[i].process_id == pid){
                        jobs[i].process_id = -1;
                        free(jobs[i].name);
                        jobs[i].name = NULL;
                        jobs[i].size = -1;
                        jobs[i].isRunning = 0;
                        jobs[i].job_id = -1;
                        runningprocess--;
                }
        }
}

int isFull(){
        return runningprocess >= JOBSIZE;
}


void add_jobs(int process_id, char** args,int size){
        // printf("passed\n");
        for (int i = 0; i < JOBSIZE; i++){
                if (jobs[i].job_id == -1){
                        jobs[i].name = malloc(sizeof(char *) * size);
                        for (int j = 0; j < size; j++){
                                jobs[i].name[j] = strdup(args[j]);

                        }
                        jobs[i].size = size;
                        jobs[i].process_id = process_id;
                        jobs[i].isRunning = 1;
                        jobs[i].job_id = i + 1;
                        runningprocess++;
                        break;
                }
        }

}

void print_jobs_name(int jobs_id){
        struct process to_print = jobs[jobs_id];
        printf("%s",GREEN);
        for (int i = 0; i < to_print.size; i++){
                printf("%s ",to_print.name[i]);
        }
        printf("%s\n",RESET);
}


int checkbgsign(char ** args, int size){
        for(int i = 0; i < size; i++){
                if (strcmp(args[i], "&") == 0){
                        return 1;
                }
        }
        return 0;
}

void jobs_cmd(){
        printf("%s| Job ID |    Status    |   Process ID  |               Command Instruction             |%s\n",WHITE,RESET);
        printf("%s----------------------------------------------------------------------------------------%s\n",WHITE,RESET);
        int containRunning = 0;
        for (int i = 0; i < JOBSIZE; i++){
            struct process process = jobs[i];
            if (process.job_id != -1){
                    if (process.isRunning){
                        printf("   %d      %sRunning%s       %s%d%s                        ", process.job_id,RED,RESET,GREEN,process.process_id,RESET);
                        print_jobs_name(i);
                        // containRunning = 1;
                    }
                    else{
                        // printf("[%d] %sStopped%s  %sProcess:%s%s%d%s  %sCommand Instruction:%s ", process.job_id,RED,RESET,PURPLE,RESET,
                                        // GREEN,process.process_id,RESET,PURPLE,RESET);
                        // print_jobs_name(i);
                        printf("   %d      %sStopped%s       %s%d%s                        ", process.job_id,RED,RESET,GREEN,process.process_id,RESET);
                        print_jobs_name(i);
                    }
                    containRunning = 1;
            }
        }
        if (!containRunning){
                printf("\n                              %s -------------------- %s                              \n",PURPLE,RESET);
                printf("                              %s|  No job is running  |%s                           \n",PURPLE,RESET);
                printf("                              %s -------------------- %s                              \n",PURPLE,RESET);
        }
        else{
            printf("%s----------------------------------------------------------------------------------------%s\n",WHITE,RESET);
        }
}

int find_jobs(char** args, int size){
        char * bg_jobid = args[size - 1];
        if (bg_jobid[0] != '%'){
                // printf("%sSpecify job id with%s %s%s%s\n",PURPLE,RESET,RED,"%",RESET);
                return -2;
        }
        int to_int = 0;
        char * jobID = strtok(bg_jobid, "%");
        while (jobID != NULL){
            to_int = atoi(jobID);
            jobID = strtok(NULL, "%");
        }
        for (int j = 0; j < JOBSIZE; j++){
                if (jobs[j].job_id == to_int){
                        print_jobs_name(j);
                        return jobs[j].process_id;
                }
        }
        return -1;
}

void set_stop_state(int process_id){
        for(int i = 0; i < JOBSIZE;i++){
                if (jobs[i].process_id == process_id){
                        jobs[i].isRunning = 0;
                }
        }
}

void set_run_state(int process_id){
        for(int i = 0; i < JOBSIZE;i++){
                if (jobs[i].process_id == process_id){
                        jobs[i].isRunning = 1;
                }
        }

}
