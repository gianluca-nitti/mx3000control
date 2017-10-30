#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <hidapi/hidapi.h>
#include "../command.h"
#include "../util.h"

static const char* options[] = {"off", "low", "middle", "high"};
static const unsigned char command_byte = 0xC;
static const char* setting_name = "led light intensity";

static int execute(int argc, char** argv, hid_device* dev) {
	return execute_simple_command(argc, argv, dev, options, command_byte, setting_name);
}

struct command get_command_setledintensity() {
	struct command result = {
		"setledintensity",
		"Set RGB led light intensity. Requires one string or integer argument: \"off\" or 1, \"low\" or 2, \"middle\" or 3, \"high\" or 4.",
		&execute
	};
	return result;
}
