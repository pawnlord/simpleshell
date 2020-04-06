#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include  <signal.h>
#include "memmanager.h"
#include "syscalls.h"

static volatile int running = 1;



void  INThandler(int sig) {
	if(!in_command){
		running = 0;
	}
}

void start_handler(void (*handler)(int sig), int SIG) {
    signal(SIG, handler);
}


int main(int argc, char** argv) {
	char** args;
	start_handler(INThandler, SIGINT);

	if(argc > 1) {
		args = argv+1;
		run(args, 1);
	}


	init_args(&args, 10, 50);
	while(running) {
		clear_args(&args);
		char* input = malloc(1000);
		printf("simpleshell: ");
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
		int d = run(args, 0);
		free(input);
	}
}