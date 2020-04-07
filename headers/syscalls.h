#ifndef SYSCALL_H
#define SYSCALL_H
int shell_init(int proc_num);

int run(char** args, int one_time, volatile int* in_command);

int change_dir(char** args);

int listproc(char** args);
#endif