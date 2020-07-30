#include <settingmanager.h>
#define NUMBEROFCOLORS 13

int color_map(char* color_name, char* color_code);

int start_settings(settings* s, char* filename){
	config cfg;
	auto_cfg_setup(&cfg);
	config_reader(filename, &cfg);
	char** field;
	
	field = malloc(50 * sizeof(char *));
	for(int i = 0; i < 50; i++){
		field[i] = malloc(MAX_SIZE);
	}
	
	char** val;
	
	val = malloc(10 * sizeof(char *));
	for(int i = 0; i < 10; i++){
		val[i] = malloc(MAX_SIZE);
	}
	get_field(cfg, "COLOR", field);
	
	get_last_attr(cfg, field, "DEFAULT", val);
	s->default_clr = malloc(30);
	color_map(val[0], s->default_clr);
	
	get_last_attr(cfg, field, "GENERAL", val);
	s->general_clr = malloc(30);
	color_map(val[0], s->general_clr);
	
	get_last_attr(cfg, field, "IMPORTANT", val);
	s->important_clr = malloc(30);
	color_map(val[0], s->important_clr);
	
	get_last_attr(cfg, field, "NAME", val);
	s->name_clr = malloc(30);
	color_map(val[0], s->name_clr);
	
	get_last_attr(cfg, field, "DIRECTORY", val);
	s->directory_clr = malloc(30);
	color_map(val[0], s->directory_clr);
	
	get_last_attr(cfg, field, "ARGUMENT", val);
	s->argument_clr = malloc(30);
	color_map(val[0], s->argument_clr);
	
	get_last_attr(cfg, field, "ERROR", val);
	s->error_clr = malloc(30);
	color_map(val[0], s->error_clr);
	
	get_last_attr(cfg, field, "ERROR_PROMPT", val);
	s->error_prompt_clr = malloc(30);
	color_map(val[0], s->error_prompt_clr);
	//printf("%simportant, %sgeneral, %s default\n", s->important_clr, s->general_clr, s->default_clr);
	for(int i = 0; i < 10; i++){
		free(val[i]);
	}
	free(val);
	for(int i = 0; i < 50; i++){
		free(field[i]);
	}
	free(field);
	return 1;
}

char color_map_colors[NUMBEROFCOLORS][20] =     {"CLEAR\0",   "BRIGHT_MAGENTA",     "BRIGHT_GREEN\0", "BRIGHT_CYAN\0", "BRIGHT_YELLOW", "BRIGHT_BLUE",  "BRIGHT_RED", "RED\0",      "BLUE",       "GREEN",      "YELLOW",      "CYAN",      "MAGENTA"};
char color_map_chars[NUMBEROFCOLORS][10] =      {"\033[0m\0", "\033[35m\0",         "\033[92m\0",     "\033[96m\0",    "\033[93m\0",    "\033[94m\0",   "\033[91m\0", "\033[31m\0", "\033[34m\0", "\033[32m\0", "\033[33m\0", "\033[36m\0", "\033[95m\0"};

int color_map(char* color_name, char* color_code){
	for(int i = 0; i < NUMBEROFCOLORS; i++){
		if(strcmp(color_name, color_map_colors[i])==0){
			strcpy(color_code, color_map_chars[i]);
			return 1;
		}
	}
	return 0;
}

int clean_settings(settings* s){
	free(s->directory_clr);
	free(s->name_clr);
	free(s->important_clr);
	free(s->general_clr);
	free(s->default_clr);
}
