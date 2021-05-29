struct process{
        char ** name;
        int size;
        int job_id;
        int process_id;
        int isRunning;
};

void init_jobs();
void free_jobs();
void remove_jobs(int pid);
int isFull();
void add_jobs(int process_id, char** args,int size);
void print_jobs_name(int jobs_id);
int checkbgsign(char ** args, int size);
void jobs_cmd();
int find_jobs(char** args, int size);
void set_stop_state(int process_id);
void set_run_state(int process_id);
int getProcessID(int job_id);
int contain_jobs(int process_id);
int isRunning(int process_id);
