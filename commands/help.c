#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <hidapi.h>
#include "../command.h"
#include "../util.h"
#include "../keymap.h"

static char* argv0;
static command_t* commands;
static int num_commands;

void init_help(char* _argv0, command_t* _commands, int _num_commands) {
	argv0 = _argv0;
	commands = _commands;
	num_commands = _num_commands;
}

static int execute(int argc, char** argv, hid_device* dev) {
	if (argc == 1) {
		wprintf(L"Usage: %s [-v | -vv] [<command>] [<command args>]\n\n"
				"Options:\n\t-v - verbose, print the unencoded HID reports which will be encoded and sent to the mouse (NOTE: some reports which are always the same and don't vary depending on user input are pre-encoded in the source of the program, so with this option you won't see everything which is sent to the mouse; see commands/setbuttons.c for more info)\n"
				"\t-vv - very verbose, print the HID reports both in unencoded format and in the encoded one (raw bytes which are sent to the mouse)\n\n", argv0);
		wprintf(L"Available commands:\n");
		for(int i = 0; i < num_commands; i++) {
			wprintf(L"\t%s - %s\n", commands[i].cmd, commands[i].description);
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

command_t get_command_help() {
	command_t result = {
		"help",
		"If invoked without arguments, shows this help message. If the string \"buttons\" is supplied as argument, shows a list of the available functions that can be assigned to mouse buttons.",
		&execute
	};
	return result;
}
