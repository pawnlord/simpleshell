#ifndef SYSCALL_H
#define SYSCALL_H
int shell_init(int proc_num);

int clean_dir(char** dir, int len, char* home_dir);

int run(char** args, int one_time, volatile int* in_command, char* home_dir);

int change_dir(char** args);

int listproc(char** args);
#endif