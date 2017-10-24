#include <stdlib.h>
#include <stdio.h>
#include <hidapi/hidapi.h>
#include "command.h"
#include "encoding.h"

static int execute(int argc, char** argv, hid_device* dev) {
	if (argc != 4) {
		fwprintf(stderr, L"Please specify 3 arguments (R, G and B values in decimal).\n");
		return 1;
	}
	unsigned char r, g, b;
	r = (char) atoi(argv[1]);
	g = (char) atoi(argv[2]);
	b = (char) atoi(argv[3]);

	wprintf(L"Setting color to R: %d, G: %d, B: %d\n", r, g, b);

	unsigned char data[] = {0, 0x7, 0xA, g, r, b, 0, 0, 0};
	encode(data);

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

struct command get_command_setledcolor() {
	struct command result = {
		"setledcolor",
		"Set RGB led color. Requires 3 numbers as arguments (R, G and B values in decimal).",
		&execute
	};
	return result;
}
