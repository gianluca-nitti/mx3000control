#ifndef _COMMAND_H
#define _COMMAND_H
#include <hidapi/hidapi.h>

struct command {
	char* cmd;
	char* description;
	int (*execute)(int, char**, hid_device*);
};

struct command get_command_raw();
struct command get_command_setledcolor();
struct command get_command_setledpulsation();
struct command get_command_setledintensity();
struct command get_command_setdpi();
struct command get_command_setsensivity();

#endif
