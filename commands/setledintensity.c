#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <hidapi/hidapi.h>
#include "../command.h"
#include "../util.h"

static int execute(int argc, char** argv, hid_device* dev) {
	if (argc != 2) {
		fwprintf(stderr, L"Please specify 1 argument.\n");
		return 1;
	}

	const char* alternatives[] = {"off", "low", "middle", "high"};
	int value = get_index_or_atoi(argv[1], alternatives, 4, "Setting led light intensity to value");
	if (value == -1) {
		return 1;
	}

	unsigned char data[] = {0, 0x7, 0xC, (unsigned char) value, 0, 0, 0, 0, 0};
	return encode_and_send_feature_report(dev, data);
}

struct command get_command_setledintensity() {
	struct command result = {
		"setledintensity",
		"Set RGB led light intensity. Requires one string or integer argument: \"off\" or 1, \"low\" or 2, \"middle\" or 3, \"high\" or 4.",
		&execute
	};
	return result;
}
