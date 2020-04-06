#include "../headers/memmanager.h"
#include <stdlib.h>
#include <string.h>

void init_args(char*** args, int s1, int s2){
	(*args) = malloc(sizeof(char*)*s1);
	for(int i = 0; i < s1; i++) {
		(*args)[i] = malloc(sizeof(char)*s2);
		for(int j = 0; j<s2; j++){
			(*args)[i][j]=0;
		}
	}
}
void free_args(char*** args, int s1){
	for(int i = 0; i < s1; i++) {
		free(*args[i]);
	}
	free(*args);
}
void clear_args(char*** argsp){
	for(int i = 0; strcmp((*argsp)[i], ""); i++){
		for(int j; (*argsp)[i][j] != 0; j++){
			(*argsp)[i][j] = 0;	
		}
	}
}