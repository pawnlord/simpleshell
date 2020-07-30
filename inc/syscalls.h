#ifndef SYSCALL_H
#define SYSCALL_H

#define MAX_ARGS 10
#define MAX_ARG_SIZE 50
#define DEFAULT_MAX_PROCESSES 1000

/* Flags for better processing */
#define BGP_FLAG 0

#include <settingmanager.h>

int shell_init(int proc_num, settings* s_);

int clean_dir(char** dir, int len, char* home_dir);

int unclean_dir(char** dir, int len, char* home_dir);

int run(char** args, int one_time, volatile int* in_command, char* home_dir, char* flags);

int change_dir(char** args);

int listproc(char** args);
#endif
