#include "../headers/syscalls.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

char* command_cd = "cd";
char* command_listproc = "listprocesses";
char* command_help = "help";

char* help_file;

char hd_char = '~';

int*   processes;
char** proc_names;
int    process_num = 1;
char   started = 0;

int shell_init(int proc_num) {
	processes = malloc(proc_num*sizeof(int));
	for(int i = 0; i < proc_num; i++){
		processes[i] = -1; /* signifies end of list */
	}
	proc_names = malloc(sizeof(char*)*proc_num);
	for(int i = 0; i < proc_num; i++){
		proc_names[i] = malloc((MAX_ARG_SIZE+1)*MAX_ARGS); 
		for(int j = 0; j < (MAX_ARG_SIZE+1)*MAX_ARGS; j++){
			proc_names[i][j] = 0;
		}
	}
	processes[0] = getpid();
	proc_names[0] = "simpleshell (this)";
	started = 1;
	help_file = malloc(1000);
	strcpy(help_file, "\
\033[91mWelcome to \033[96msimpleshell\033[91m!\033[0m\n\
This is a shell made for fun and for the sake of trying to replicate other shell features\n\
if you where expecting a serious shell, well too bad!\n\
\033[91musage:\n\
\033[94msimpleshell \033[96m[<command...>]\033[0m\n\
\033[91mbuiltin commands:\n\
- \033[94mhelp:\033[0m get help on general usage and commands\n\
- \033[94mcd\033[0m \033[96m<directory>\033[0m: change working directory to <directory>. \n\tIf <directory> is left blank, it will go to your home directory\n\
- \033[94mlistprocesses\033[0m: list all processes spun by this current shell\n");
}

int clean_dir(char** dir, int len, char* home_dir){
	int diff = 0;
	int i;
	for(i = 0; (*dir)[i] != 0 && home_dir[i] != 0; i++) {
		if((*dir)[i] != home_dir[i]){
			diff = 1;
			break;
		}
	}
	if((*dir)[i] == 0 && home_dir[i] != 0){
		diff = 1;
	}
	int hd_len = i;
	if(diff == 0){
		char* dir_copy = malloc(len);
		for(i = hd_len; (*dir)[i] != 0; i++){
			dir_copy[i-hd_len] = (*dir)[i];
		}
	
		(*dir)[0] = hd_char;
		for(i = 0; dir_copy[i] != 0; i++){
			(*dir)[i+1] = dir_copy[i];
		}
		i++;
		for(; (*dir)[i] != 0; i++){
			(*dir)[i] = 0;
		}
		free(dir_copy);
		
	}
}
int unclean_dir(char** dir, int len, char* home_dir){
	if((*dir)!=NULL){
		if((*dir)[0] == hd_char){
			char* dir_copy = malloc(len);
			int i;
			for(i = 1; (*dir)[i] != 0; i++){
				dir_copy[i-1] = (*dir)[i];
			}
			int cd_len = i-1;
			for(i = 0; home_dir[i] != 0 && i < len; i++){
				(*dir)[i] = home_dir[i];
			}
			int hd_len = i;
			for(i = 0; dir_copy[i] != 0; i++){
				(*dir)[hd_len+i] = dir_copy[i];
			}
			(*dir)[hd_len+i] = 0;
		}
		return 1;
	}
	printf("simpleshell: unclean_dir: dir is NULL!\n");
	return 0;
}

int run(char** args, int one_time, volatile int* in_command, char* home_dir) {
	
	/* find end */
	int is_bgprocess = 0;
	int end_of_args;
	for(end_of_args = 0; strcmp(args[end_of_args], ""); end_of_args++){
		unclean_dir(args+end_of_args, 50, home_dir);
		if(end_of_args != 0){
			if(strcmp(args[end_of_args], "&") == 0 &&
				strcmp(args[end_of_args+1], "") == 0){
				is_bgprocess = 1;
				break;			
			}
		}
	}
	
	
	if(end_of_args == 0){
		return 0;
	}
	
	char* eoasave = args[end_of_args];
	args[end_of_args] = NULL;
	
	
	if(strcmp(args[0], command_cd) == 0){
		if(args[1] == NULL && end_of_args==1){
			args[end_of_args] = eoasave;
			end_of_args+=1;
			eoasave = args[end_of_args];
			args[end_of_args] = NULL;
			strcpy(args[1], home_dir);
		}
		change_dir(args+1);
		args[end_of_args] = eoasave;
		return 0;
	}
	if(strcmp(args[0], command_listproc) == 0 && started == 1){
		listproc(args+1);
		args[end_of_args] = eoasave;
		return 0;
	}
	if(strcmp(args[0], command_help) == 0 && started == 1){
		help(args+1);
		args[end_of_args] = eoasave;
		return 0;
	}
	
	if(one_time) { /* this is one time, do not fork */
		execvp(args[0], args);
		return -1;
	} 
	
	
	int process = fork();
	if(process == 0) {	/* we are child process */
		int process_index = process_num;
		int succ = execvp(args[0], args);
		if(succ < 0){
			char* message = malloc(100);
			sprintf(message, "\033[92msimpleshell:\033[0m \033[94m%s\033[0m", args[0]);
			perror(message);
			fflush(stdout);
			fflush(stderr);
			free(message);
		}
		if(is_bgprocess){
			printf("\nsimpleshell: process %d killed\n", processes[process_index]);
			processes[process_index] = -2;
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
				char* process_name = malloc((MAX_ARG_SIZE+1)*MAX_ARGS);
				strcpy(process_name, args[0]);
				for(int i = 1; args[i] != NULL; i++){
					strcat(process_name, " ");
					strcat(process_name, args[i]);
				}
				strcpy(proc_names[process_num], process_name);
				process_num++;
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
		printf("\033[94m[%d]\033[0m %s\n", processes[i], proc_names[i]);
	}
}

int help(char** args){
	int i = 0;
	/* do checks for params here */
	printf("%s\n", help_file);
}