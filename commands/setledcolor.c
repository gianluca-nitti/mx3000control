#include <stdlib.h>
#include <stdio.h>
#include <hidapi/hidapi.h>
#include "../command.h"
#include "../util.h"

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
	return encode_and_send_feature_report(dev, data);
}

struct command get_command_setledcolor() {
	struct command result = {
		"setledcolor",
		"Set RGB led color. Requires 3 numbers as arguments (R, G and B values in decimal).",
		&execute
	};
	return result;
}
