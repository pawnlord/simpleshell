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
#include "../headers/termfuncs.h"

#define BIG_STR_SIZE 1000
#define SMALL_STR_SIZE 100

static volatile int* sigint_flag;

static volatile int* in_command;


void  INThandler(int sig) {
	if(!(*in_command)){
		*sigint_flag = 0;
	}
}

void start_handler(void (*handler)(int sig), int SIG) {
    signal(SIG, handler);
}

int flaggedreadin(char* buff, volatile int* flag, char eol) {
	char c = 0;;
	int buff_counter = 0;
	if(buff == NULL){
		printf("simpleshell: input: err, buff is null\n");
		return 0;
	}
	while(c!=eol && *flag!=0){
		c=getch();
		if(c == 0x7f && buff_counter!=0){
			buff_counter-=1;
			buff[buff_counter] = 0;
			delete(1);
		} else if(c != 0x7f){
			printf("%c", c);
			buff[buff_counter] = c;
			buff_counter++;
		}
	}
}

int main(int argc, char** argv) {
	char** args;
	
	int sigint_flagval = 1;
	int in_commandval = 0;
	sigint_flag = &sigint_flagval;
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
	
	while(1) {
		clear_args(&args, MAX_ARGS, MAX_ARG_SIZE);
		char* input = malloc(BIG_STR_SIZE);
		for(int i = 0; i < BIG_STR_SIZE; i++){
			input[i] = 0;
		}
		char* dir = malloc (SMALL_STR_SIZE);
		getcwd(dir, SMALL_STR_SIZE);
		clean_dir(&dir, SMALL_STR_SIZE, homedir);
		printf("\033[92m[simpleshell]\033[93m%s:\033[91m%s\033[0m$ ", usrname, dir);
		fflush(stdout);
		flaggedreadin(input, sigint_flag, '\n');
		input[strcspn(input, "\n")] = '\0';
		
		int current_arg = 0;
		int args_char_counter = 0;
		int i;
		int escaped = 0;
		for(i = 0; input[i] != 0 && !(!escaped && input[i] == '#'); i++) {
			if(input[i] == ' ') {
				current_arg++;
				args_char_counter = 0;
				escaped = 0;
			} else if(input[i] == '\\' && !escaped){
				escaped = 1;
			} else {
				args[current_arg][args_char_counter] = input[i];
				args_char_counter++;
				escaped = 0;
			}
		}
		strcpy(args[++current_arg], "");
		if(strcmp(args[0], "exit") == 0){
			break;
		}
		int d = run(args, 0, in_command, homedir);
		*sigint_flag = 1;
		free(input);
	}
}