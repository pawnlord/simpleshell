#include "../headers/syscalls.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
char* command_cd = "cd";
char* command_listproc = "listprocesses";

int*  processes;
int   process_num = 1;
char  started = 0;

int shell_init(int proc_num) {
	processes = malloc(proc_num*sizeof(int));
	for(int i = 0; i < proc_num; i++){
		processes[i] = -1; /* signifies end of list */
	}
	processes[0] = getpid();
	started = 1;
}

int run(char** args, int one_time, volatile int* in_command) {
	if(one_time) { /* this is one time, do not fork */
		execvp(args[0], args);
		return -1;
	} 
	/* find end */
	int is_bgprocess = 0;
	int end_of_args;
	for(end_of_args = 0; strcmp(args[end_of_args], ""); end_of_args++){
		if(end_of_args != 0){
			if(strcmp(args[end_of_args], "&") == 0 &&
				strcmp(args[end_of_args+1], "") == 0){
				is_bgprocess = 1;
				break;			
			}
		}
	}
	char* eoasave = args[end_of_args];
	args[end_of_args] = NULL;
	
	if(strcmp(args[0], command_cd) == 0){
		change_dir(args+1);
		args[end_of_args] = eoasave;
		return 0;
	}
	if(strcmp(args[0], command_listproc) == 0 && started == 1){
		listproc(args+1);
		args[end_of_args] = eoasave;
		return 0;
	}
	
	int process = fork();
	if(process == 0) {	/* we are child process */
		int succ = execvp(args[0], args);
		if(succ < 0){
			char* message = malloc(100);
			sprintf(message, "simpleshell: %d %s", is_bgprocess, args[0]);
			perror(message);
			fflush(stdout);
			fflush(stderr);
			free(message);
		}
		exit(0);
	} else if (process < 0) { /* we failed, exit */
		perror("simpleshell: run");
		exit(EXIT_FAILURE);
	} else { /* we are parent, wait until child exits */
		if(!is_bgprocess){
			(*in_command) = 1;
			waitpid(process, NULL, 0);
			(*in_command) = 0;
		} else {
			printf("new process id: %d\n", process);
			if(started == 1){
				processes[process_num] = process;			
			}
			is_bgprocess = 0;
		}
	}
	args[end_of_args] = eoasave;
	return process;
}

int change_dir(char** args){
	int i = 0;
	/* do checks for params here */
	chdir(args[i]);
}

int listproc(char** args){
	int i = 0;
	/* do checks for params here */
	for(i = 0; processes[i] != -1; i++){
		printf("[%d]\n", processes[i]);
	}
}