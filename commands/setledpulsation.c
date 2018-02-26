#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <hidapi.h>
#include "../command.h"
#include "../util.h"

static const char* options[] = {"steady", "slow", "middle", "fast"};
static const int n_options = 4;
static const unsigned char command_byte = 0x0B;
static const char* setting_name = "led pulsation effect";

static int execute(int argc, char** argv, hid_device* dev) {
	return execute_simple_command(argc, argv, dev, options, n_options, command_byte, setting_name);
}

command_t get_command_setledpulsation() {
	command_t result = {
		"setledpulsation",
		"Set RGB led pulsation effect. Requires one string or integer argument: \"steady\" or 1, \"slow\" or 2, \"middle\" or 3, \"fast\" or 4.",
		&execute
	};
	return result;
}
