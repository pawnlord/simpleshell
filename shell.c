#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int run(char** args) {
	execvp(args[0], args);
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

int main(int argc, char** argv) {
	char** args;
	if(argc > 1) {
		args = argv+1;
		run(args);
	}
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
	args[++current_arg] = NULL;
	
	run(args);
	printf("Hello!");
}