#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include "../headers/memmanager.h"
#include "../headers/syscalls.h"

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
	
	start_handler(INThandler, SIGINT);

	if(argc > 1) {
		args = argv+1;
		run(args, 1, NULL);
	}

	shell_init(1000);
	init_args(&args, 10, 50);
	
	while(running) {
		clear_args(&args, 10, 50);
		char* input = malloc(1000);
		printf("HELP\n");
		printf("simpleshell: ");
		fflush(stdout);
		fgets(input, 1000, stdin);
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
		int d = run(args, 0, in_command);
		
		free(input);
	}
}