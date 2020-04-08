#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pwd.h>
#include <signal.h>

#include "../headers/memmanager.h"
#include "../headers/syscalls.h"

#define BIG_STR_SIZE 1000
#define SMALL_STR_SIZE 100

static volatile int* running;

static volatile int* in_command;


void  INThandler(int sig) {
	if(!(*in_command)){
		*running = 0;
	}
}

void start_handler(void (*handler)(int sig), int SIG) {
    signal(SIG, handler);
}



int main(int argc, char** argv) {
	char** args;
	
	int runningval = 1;
	int in_commandval = 0;
	running = &runningval;
	in_command = &in_commandval;
	
	struct passwd *pw = getpwuid(getuid());
	char* homedir = malloc(SMALL_STR_SIZE);
	strcpy(homedir, pw->pw_dir);
	
	char* usrname = pw->pw_name;
	start_handler(INThandler, SIGINT);

	if(argc > 1) {
		args = argv+1;
		run(args, 1, NULL, homedir);
	}

	shell_init(DEFAULT_MAX_PROCESSES);
	init_args(&args, MAX_ARGS, MAX_ARG_SIZE);
	
	while(running) {
		clear_args(&args, MAX_ARGS, MAX_ARG_SIZE);
		char* input = malloc(BIG_STR_SIZE);
		char* dir = malloc (SMALL_STR_SIZE);
		getcwd(dir, SMALL_STR_SIZE);
		clean_dir(&dir, SMALL_STR_SIZE, homedir);
		printf("[simpleshell]%s:%s$ ", usrname, dir);
		fflush(stdout);
		fgets(input, BIG_STR_SIZE, stdin);
		input[strcspn(input, "\n")] = '\0';
		
		int current_arg = 0;
		int args_char_counter = 0;
		int i;
		
		for(i = 0; input[i] != 0; i++) {
			if(input[i] == ' ') {
				current_arg++;
				args_char_counter = 0;
			} else {
				args[current_arg][args_char_counter] = input[i];
				args_char_counter++;
			}
		}
		
		strcpy(args[++current_arg], "");
		if(strcmp(args[0], "exit") == 0){
			break;
		}
		int d = run(args, 0, in_command, homedir);
		
		free(input);
	}
}