#!/bin/bash

gcc src/shell.c src/syscalls.c src/memmanager.c src/termfuncs.c -o simpleshell.o -Iinc -Llib -lconfigreader

