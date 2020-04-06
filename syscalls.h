#ifndef SYSCALL_H
#define SYSCALL_H

static volatile int in_command = 0;

int run(char** args, int one_time);

int change_dir(char** args);
#endif