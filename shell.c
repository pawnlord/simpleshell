#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int run(char** args, int one_time) {
	if(one_time) { /* this is one time, do not fork */
		printf("ONE TIME!\n");
		execvp(args[0], args);
		return 0;
	}
	int process = fork();
	if(process == 0) {	/* we are child process */
		printf("CHILD EXECUTING!\n");
		execvp(args[0], args);
	} else if (process < 0) { /* we failed, exit */
		perror("simpleshell: FORK ERROR! EXITING\n");
		exit(EXIT_FAILURE);
	} else { /* we are parent, wait until child exits */
		printf("PARENT WAITING!\n");
		wait(NULL);
	}
}

void init_args(char*** args, int s1, int s2){
	char** array = (char**) malloc(sizeof(char*)*s1);
	for(int i = 0; i < s1; i++) {
		array[i] = (char*) malloc(sizeof(char)*s2);
		for(int j = 0; j<s2; j++){
			array[i][j]=0;
		}
	}
	*args = array;
}
void clear_args(char*** argsp){
	char** args = *argsp;
	for(int i = 0; strcmp(args[i], ""); i++){
		for(int j; args[i][j] != 0; j++){
			args[i][j] = 0;	
		}
	}
	*argsp = args;
}


int main(int argc, char** argv) {
	char** args;
	if(argc > 1) {
		args = argv+1;
		run(args, 1);
	}
	init_args(&args, 10, 50);
	while(1) {
		init_args(&args, 10, 50);
		char* input = malloc(1000);
		printf("simpleshell: ");
		gets(input);
		printf("Acquired input: %s\n", input);
		int current_arg = 0;
		int args_char_counter = 0;
		for(int i = 0; input[i] != 0; i++) {
			if(input[i] == ' ') {
				current_arg++;
				args_char_counter = 0;
			} else {
				args[current_arg][args_char_counter] = input[i];
				args_char_counter++;
			}
		}
		args[++current_arg] = "";
		if(strcmp(args[0], "exit") == 0)
			break;
		run(args, 0);
	}
}