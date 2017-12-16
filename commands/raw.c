#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <hidapi.h>
#include "../command.h"
#include "../util.h"

static int execute(int argc, char** argv, hid_device* dev) {
	if (argc != 10) {
		fwprintf(stderr, L"Please specify 9 arguments (type - \"feature\" or \"output\" - followed by 8 bytes to send in hex format).\n");
		return 1;
	}
	unsigned char data[9];
	data[0] = 0;
	for (int i = 1; i < 9; i++) {
		data[i] = (char) strtol(argv[i + 1], NULL, 16);
	}
	if (strcmp("feature", argv[1]) == 0) {
		return send_report(dev, data, FEATURE_REPORT);
	} else if (strcmp("output", argv[1]) == 0) {
		return send_report(dev, data, OUTPUT_REPORT);
	} else {
		fwprintf(stderr, L"First argument to the \"raw\" command must be \"feature\" or \"output\".");
		return 1;
	}
}

command_t get_command_raw() {
	command_t result = {
		"raw",
		"Send raw bytes from command line (in hex format) as HID output report or feature report.",
		&execute
	};
	return result;
}
