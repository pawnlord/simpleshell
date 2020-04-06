#ifndef SYSCALL_H
#define SYSCALL_H

int run(char** args, int one_time, volatile int* in_command);

int change_dir(char** args);
#endif