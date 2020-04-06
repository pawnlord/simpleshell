#include "../headers/syscalls.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

char* command_cd = "cd";

int run(char** args, int one_time, volatile int* in_command) {
	if(one_time) { /* this is one time, do not fork */
		execvp(args[0], args);
		return -1;
	}
	int end_of_args;
	for(end_of_args = 0; strcmp(args[end_of_args], ""); end_of_args++){} /* find end */
	char* eoasave = args[end_of_args];
	args[end_of_args] = NULL;
	
	if(strcmp(args[0], command_cd) == 0){
		change_dir(args+1);
		args[end_of_args] = eoasave;
		return 0;
	}
	
	int process = fork();
	if(process == 0) {	/* we are child process */
		int succ = execvp(args[0], args);
		if(succ < 0){
			char* message = malloc(100);
			sprintf(message, "simpleshell: %s", args[0]);
			perror(message);
			free(message);
		}
		exit(0);
	} else if (process < 0) { /* we failed, exit */
		perror("simpleshell: run");
		exit(EXIT_FAILURE);
	} else { /* we are parent, wait until child exits */
		(*in_command) = 1;
		wait(NULL);
		(*in_command) = 0;
	}
	args[end_of_args] = eoasave;
	return process;
}

int change_dir(char** args){
	int i = 0;
	/* do checks for params here */
	chdir(args[i]);
}