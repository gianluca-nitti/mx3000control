#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <hidapi.h>
#include "../command.h"
#include "../util.h"

static const char* options[] = {"600", "1200", "2000", "3000", "4800", "6400", "8200"};
static const int n_options = 7;
static const unsigned char command_byte = 0x09;
static const char* setting_name = "sensor DPI";

static int execute(int argc, char** argv, hid_device* dev) {
	return execute_simple_command(argc, argv, dev, options, n_options, command_byte, setting_name);
}

command_t get_command_setdpi() {
	command_t result = {
		"setdpi",
		"Set sensor DPI. Requires one argument: \"600\" or 1, \"1200\" or 2, \"2000\" or 3, \"3000\" or 4, \"4800\" or 5, \"6400\" or 6, \"8200\" or 7.",
		&execute
	};
	return result;
}
