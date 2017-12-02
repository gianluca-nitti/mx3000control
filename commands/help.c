#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <hidapi/hidapi.h>
#include "../command.h"
#include "../util.h"
#include "../keymap.h"

static struct command* commands;
static int num_commands;

void init_help(struct command* _commands, int _num_commands) {
	commands = _commands;
	num_commands = _num_commands;
}

static int execute(int argc, char** argv, hid_device* dev) {
	if (argc == 1) {
		wprintf(L"Available commands:\n");
		for(int i = 0; i < num_commands; i++) {
			wprintf(L"%s - %s\n", commands[i].cmd, commands[i].description);
		}
		return 0;
	} else if (argc == 2 && strcmp("buttons", argv[1]) == 0) {
		wprintf(L"The following values can be used as arguments for the \"setbuttons\" command:\n");
		for (int i = 0; i < numKeys; i++) {
			wprintf(L"%s\n", keys[i]);
		}
		return 0;
	}
	fwprintf(stderr, L"The \"help\" command can be called without arguments to show a list of the available commands, or with the string \"buttons\" as the only argument to show a list of the available functions which can be assigned to the mouse buttons.\n");
	return 1;
}

struct command get_command_help() {
	struct command result = {
		"help",
		"Show this help message.",
		&execute
	};
	return result;
}
