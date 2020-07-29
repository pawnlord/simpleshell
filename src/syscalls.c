#include <syscalls.h>
#include <termfuncs.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

/* builtin commands */
char* command_cd = "cd";
char* command_listproc = "listprocesses";
char* command_help = "help";
char* command_clear = "clear";

/* text to print for help command */
char* help_file;

/* character that reperesents the home directory */
char hd_char = '~';

/* lists of processes made by this program */
int*   processes;
char** proc_names;
/* current process */
int    process_num = 1;
/* tells us whether shell_init has been called */
char   started = 0;

int shell_init(int proc_num) {
	/* allocate and clear some arrays */
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
	
	/* put our PID and name first in lists */
	processes[0] = getpid();
	proc_names[0] = "simpleshell (this)";
	/* set started to true */
	started = 1;
	
	/* initialize help text */
	help_file = malloc(1000);
	strcpy(help_file, "\
\033[91mWelcome to \033[96msimpleshell\033[91m!\033[0m\n\
This is a shell made for fun and for the sake of trying to replicate other shell features\n\
if you where expecting a serious shell, well too bad!\n\
\033[91musage:\n\
\033[94msimpleshell \033[96m[<command...>]\033[0m\n\
\033[91mbuiltin commands:\033[0m\n\
- \033[94mhelp\033[0m: get help on general usage and commands\n\
- \033[94mcd\033[0m \033[96m<directory>\033[0m: change working directory to <directory>. \n\tIf <directory> is left blank, it will go to your home directory\n\
- \033[94mlistprocesses\033[0m: list all processes spun by this current shell\n\
\033[91msyntax:\033[0m\n\
- \033[94m&\033[0m: when put as the last arguement, commands preceeding it are ran as background processes.\n\
- \033[94m#\033[0m: stops arguement reading. is a comment.\n\
- \033[94m\\\033[0m: escapes next character if the character is a syntax character.\n");

}

/* make dir with home directory replaced with ~ */
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

/* replace ~ with home directory */
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

/* run commands */
int run(char** args, int one_time, volatile int* in_command, char* home_dir, char* flags) {
	
	/* find end */
	int is_bgprocess = flags[BGP_FLAG];
	int end_of_args;
	for(end_of_args = 0; strcmp(args[end_of_args], ""); end_of_args++){
		unclean_dir(args+end_of_args, 50, home_dir);
	}
	
	/* if there is nothing, return */
	if(end_of_args == 0){
		return 0;
	}
	
	/* set the end to NULL, but save original position */
	char* eoasave = args[end_of_args];
	args[end_of_args] = NULL;
	
	/* if it cd, we know to use change_dir */
	if(strcmp(args[0], command_cd) == 0){
		/* default to the home directory */
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
	/* listprocesses lists all processes made by the shell, with names */
	if(strcmp(args[0], command_listproc) == 0 && started == 1){
		listproc(args+1);
		args[end_of_args] = eoasave;
		return 0;
	}
	/* list help for commands */
	if(strcmp(args[0], command_help) == 0 && started == 1){
		help(args+1);
		args[end_of_args] = eoasave;
		return 0;
	}
	
	/* clear terminal */
	if(strcmp(args[0], command_clear) == 0 && started == 1){
		clear();
		args[end_of_args] = eoasave;
		return 0;
	}
	/* if its a one time, do it one time */
	if(one_time) { /* this is one time, do not fork */
		execvp(args[0], args);
		return -1;
	} 
	
	/* fork a child process to be swiftly killed */
	int process = fork();
	if(process == 0) {	/* we are child process */
		
		int process_index = process_num;
		/* execute command */
		int succ = execvp(args[0], args);
		/* if there was an error, print it */
		if(succ < 0){
			char* message = malloc(100);
			sprintf(message, "\033[92msimpleshell:\033[0m \033[94m%s\033[0m", args[0]);
			perror(message);
			fflush(stdout);
			fflush(stderr);
			free(message);
		}
		/* if this was a background process, tell us it was killed */
		if(is_bgprocess){
			printf("\nsimpleshell: process %d killed\n", processes[process_index]);
			processes[process_index] = -2;
		}
		/* exit */
		exit(0);
	} else if (process < 0) { /* we failed, exit */
		perror("simpleshell: run");
		exit(EXIT_FAILURE);
	} else { /* we are parent, wait until child exits */
		if(!is_bgprocess){ 
			/* wait for command to end */
			(*in_command) = 1;
			waitpid(process, NULL, 0);
			(*in_command) = 0;
		} else {  /* we don't have to wait if we are a background process */
			/* tells us the process id */
			printf("new process id: %d\n", process);
			/* if we used shell_init, save the process to the lists */
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
			/* reset is_bgprocess */
			is_bgprocess = 0;
		}
	}
	/* take the null arg and return it to the correct pointer */
	args[end_of_args] = eoasave;
	return process;
}

/* a syscall, we might want to do some creative stuff here */
int change_dir(char** args){
	int i = 0;
	/* do checks for params here */
	
	chdir(args[i]);
}

/* list processes started by this */
int listproc(char** args){
	int i = 0;
	/* do checks for params here */
	for(i = 0; processes[i] != -1; i++){
		printf("\033[94m[%d]\033[0m %s\n", processes[i], proc_names[i]);
	}
}

/* display help */
int help(char** args){
	int i = 0;
	/* do checks for params here */
	printf("%s\n", help_file);
}
