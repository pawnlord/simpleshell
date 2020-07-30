#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pwd.h>
#include <signal.h>

#include <memmanager.h>
#include <syscalls.h>
#include <termfuncs.h>

#include <settingmanager.h>

/* general size for strings */
#define BIG_STR_SIZE 1000
#define SMALL_STR_SIZE 100

static volatile int* sigint_flag;

static volatile int* in_command;

/* handler for SIGINT, sets sigint_flag to 1 if we are not in a command */
void  INThandler(int sig) {
	if(!(*in_command)){
		*sigint_flag = 1;
	}
}

/* start a handler */
void start_handler(void (*handler)(int sig), int SIG) {
    signal(SIG, handler);
}

int flaggedreadin(char* buff, volatile int* flag, char eol) {
	char c = 0;
	int buff_counter = 0;
	/* error if buff is null */
	if(buff == NULL){
		printf("simpleshell: input: err, buff is null\n");
		return 0;
	}
	/* wait until end of line or flag is 1 (SIGINT) */
	while(c!=eol && (*flag)!=1){
		c=getch();
		if(c == 0x7f && buff_counter!=0){
			buff_counter-=1;
			buff[buff_counter] = 0;
			delete(1);
		} else if(c != 0x7f && (*flag)!=1){
			printf("%c", c);
			buff[buff_counter] = c;
			buff_counter++;
		}
	}
}

int main(int argc, char** argv) {
	/* arguements we pass to the run function*/
	char** args;
	
	/* values that flags point to */
	int sigint_flagval = 0;
	int in_commandval = 0;
	
	/* flag pointers that we pass to other things */
	sigint_flag = &sigint_flagval;
	in_command = &in_commandval;
	
	/* for user info */
	struct passwd *pw = getpwuid(getuid());
	
	/* get home directory*/
	char* homedir = malloc(SMALL_STR_SIZE);
	strcpy(homedir, pw->pw_dir);
	char* cfg_homedir;
	if((cfg_homedir = getenv("XDG_CONFIG_HOME")) == NULL){
		cfg_homedir = malloc(100);
		for(int i = 0; i < 100; i++){
			cfg_homedir[i] = 0;
		}
		if(getenv("HOME")==NULL){
			strcat(strcat(cfg_homedir, homedir), "/.config");	
		} else{
			printf("HOME FOUND\n");
			strcat(strcat(cfg_homedir, getenv("HOME")), "/.config");
		}
	} 
	
	settings s;
	start_settings(&s, strcat(cfg_homedir, "/simpleshell.cfg"));
	
	
	/* get the current username */
	char* usrname = pw->pw_name;
	
	start_handler(INThandler, SIGINT);
	/* for running a single command through the shell */
	if(argc > 1) {
		args = argv+1;
		run(args, 1, NULL, homedir, NULL);
	}
	/* a welcoming */
	printf("%sWelcome to %ssimpleshell%s!\n", s.general_clr, s.important_clr, s.general_clr);
	printf("Type %shelp%s for commands\n", s.important_clr, s.general_clr);
	
	/* initialize shell arrays */
	shell_init(DEFAULT_MAX_PROCESSES, &s);
	/* allocate args */
	init_args(&args, MAX_ARGS, MAX_ARG_SIZE);
	/* setup flags*/
	char* flags = malloc(SMALL_STR_SIZE);
	for(int i = 0; i < SMALL_STR_SIZE; i++){
		flags[i] = 0;
	}
	/* main loop */
	while(1) {
		/* reset arguements back to zero */
		clear_args(&args, MAX_ARGS, MAX_ARG_SIZE);
		
		/* general itterator for for-loops */
		int i;
		
		/* reset flags */
		for(i = 0; i < SMALL_STR_SIZE; i++){
			flags[i] = 0;
		}
		
		/* reset input back to zero */
		char* input = malloc(BIG_STR_SIZE);
		for(i = 0; i < BIG_STR_SIZE; i++){
			input[i] = 0;
		}
		
		/* cget the current dir */
		char* dir = malloc (SMALL_STR_SIZE);
		getcwd(dir, SMALL_STR_SIZE);
		/* clean dir, set all homedirs to ~s*/
		clean_dir(&dir, SMALL_STR_SIZE, homedir);
		/* print prompt "[simpleshell]username:directory$" */
		printf("%s[simpleshell]%s%s:%s%s%s$ ", s.general_clr, s.name_clr, usrname,s.directory_clr, dir, s.default_clr);
		fflush(stdout);
		/* readin using special function */
		flaggedreadin(input, sigint_flag, '\n');
		/* if there wasn't a SIGINT, execute the command */
		if(!(*sigint_flag)){
			/* remove eol */
			input[strcspn(input, "\n")] = '\0';
			/* definitions */
			int current_arg = 0;
			int args_char_counter = 0;
			int escaped = 0;
			/* loop through ibput, parsing it for arguements and syntax */
			for(i = 0; input[i] != 0 && !(!escaped && input[i] == '#'); i++) {
				if(input[i] == ' ') { /* arguements are seperated by spaces */
					current_arg++;
					args_char_counter = 0;
					escaped = 0;
				} else if(input[i] == '\\' && !escaped){ /* escape character */
					escaped = 1; 
					flags[BGP_FLAG] = 0;
				} else if(input[i] == '&' && !escaped){ /* background process character */
					flags[BGP_FLAG] = 1;
					escaped = 0;
				} else { /* default, if the character is normal or escaped */
				
					flags[BGP_FLAG] = 0;
					args[current_arg][args_char_counter] = input[i];
					args_char_counter++;
					escaped = 0;
				}
			}
			strcpy(args[++current_arg], ""); /* put terminator at the end of args */
			/* if the command is exit, break */
			if(strcmp(args[0], "exit") == 0){
				break;
			}
			/* run the command */
			int d = run(args, 0, in_command, homedir, flags);
		} else {
			/* else, print a newline and continue */
			printf("\n");
		}
		/* reset flag and input */
		*sigint_flag = 0;
		free(input);
	}
	clean_settings(&s);
}
