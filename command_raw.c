#include <stdlib.h>
#include <stdio.h>
#include <hidapi/hidapi.h>
#include "command.h"

int execute(int argc, char** argv, hid_device* dev) {
	if (argc != 9) {
		fwprintf(stderr, L"Please specify 8 arguments (8 bytes of feature report to send in hex format).\n");
		return 1;
	}

	/* send feature report specified on command line (8 bytes, 1 for each argument, in hex) */
	unsigned char data[9];
	data[0] = 0;
	for (int i = 1; i < 9; i++) {
		data[i] = (char) strtol(argv[i], NULL, 16);
	}
	wprintf(L"Feature report to send: %X %X %X %X %X %X %X %X %X\n", data[0], data[1],
			data[2], data[3], data[4], data[5], data[6], data[7], data[8]);

	int status = hid_send_feature_report(dev, data, 9);
	if (status == -1) {
		wprintf(L"Failed to send feature report: \"%S\".\n", hid_error(dev));
	} else {
		wprintf(L"Feature report sent.\n");
	}
	return 0;
}

struct command get_command_raw() {
	struct command result = {
		"raw",
		"Send raw bytes from command line (in hex format) as HID feature report",
		&execute
	};
	return result;
}
