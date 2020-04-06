#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include  <signal.h>

static volatile int running = 1;

static volatile int in_command = 0;

int run(char** args, int one_time) {
	if(one_time) { /* this is one time, do not fork */
		execvp(args[0], args);
		return -1;
	}
	int end_of_args;
	for(end_of_args = 0; strcmp(args[end_of_args], ""); end_of_args++){} /* find end */
	char* eoasave = args[end_of_args];
	args[end_of_args] = NULL;
	int process = fork();
	if(process == 0) {	/* we are child process */
		execvp(args[0], args);
		exit(0);
	} else if (process < 0) { /* we failed, exit */
		perror("simpleshell: run");
		exit(EXIT_FAILURE);
	} else { /* we are parent, wait until child exits */
		in_command = 1;
		wait(NULL);
		in_command = 0;
	}
	printf("end: %d\n", args[1]);
	args[end_of_args] = eoasave;
	return process;
}

void init_args(char*** args, int s1, int s2){
	(*args) = malloc(sizeof(char*)*s1);
	for(int i = 0; i < s1; i++) {
		(*args)[i] = malloc(sizeof(char)*s2);
		for(int j = 0; j<s2; j++){
			(*args)[i][j]=0;
		}
	}
}
void free_args(char*** args, int s1){
	for(int i = 0; i < s1; i++) {
		free(*args[i]);
	}
	free(*args);
}
void clear_args(char*** argsp){
	for(int i = 0; strcmp((*argsp)[i], ""); i++){
		for(int j; (*argsp)[i][j] != 0; j++){
			(*argsp)[i][j] = 0;	
		}
	}
}

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