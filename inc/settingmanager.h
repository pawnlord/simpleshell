#ifndef SETTINGMANAGER_H
#define SETTINGMANAGER_H
/* general size for strings */
#define BIG_STR_SIZE 1000
#define SMALL_STR_SIZE 100
#include <config_reader.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct{
	char* directory_clr;
	char* general_clr;
	char* name_clr;
	char* important_clr;
	char* default_clr;

} settings;

int start_settings(settings* s, char* filename); 

int clean_settings(settings* s); 

#endif
