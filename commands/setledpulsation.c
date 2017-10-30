#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <hidapi/hidapi.h>
#include "../command.h"
#include "../util.h"

static const char* options[] = {"steady", "slow", "middle", "fast"};
static const unsigned char command_byte = 0xB;
static const char* setting_name = "led pulsation effect";

static int execute(int argc, char** argv, hid_device* dev) {
	return execute_simple_command(argc, argv, dev, options, command_byte, setting_name);
}

struct command get_command_setledpulsation() {
	struct command result = {
		"setledpulsation",
		"Set RGB led pulsation effect. Requires one string or integer argument: \"steady\" or 1, \"slow\" or 2, \"middle\" or 3, \"fast\" or 4.",
		&execute
	};
	return result;
}
