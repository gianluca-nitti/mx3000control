#ifndef _COMMAND_H
#define _COMMAND_H
#include <hidapi.h>

typedef struct {
	char* cmd;
	char* description;
	int (*execute)(int, char**, hid_device*);
} command_t ;

void init_help(char*, command_t*, int);
command_t get_command_help();
command_t get_command_raw();
command_t get_command_setledcolor();
command_t get_command_setledpulsation();
command_t get_command_setledintensity();
command_t get_command_setdpi();
command_t get_command_setsensivity();
command_t get_command_setbuttons();
command_t get_command_encode();

#endif
