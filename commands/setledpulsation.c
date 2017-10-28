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

	const char* alternatives[] = {"steady", "slow", "middle", "fast"};
	int value = get_index_or_atoi(argv[1], alternatives, 4, "Setting led pulsation effect to value");
	if (value == -1) {
		return 1;
	}

	unsigned char data[] = {0, 0x7, 0xB, (unsigned char) value, 0, 0, 0, 0, 0};
	return encode_and_send_feature_report(dev, data);
}

struct command get_command_setledpulsation() {
	struct command result = {
		"setledpulsation",
		"Set RGB led pulsation effect. Requires one string or integer argument: \"steady\" or 1, \"slow\" or 2, \"middle\" or 3, \"fast\" or 4.",
		&execute
	};
	return result;
}
