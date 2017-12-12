#include <stdlib.h>
#include <stdio.h>
#include <hidapi.h>
#include "../command.h"
#include "../util.h"

static int execute(int argc, char** argv, hid_device* dev) {
	if (argc != 4) {
		fwprintf(stderr, L"Please specify 3 arguments (R, G and B values in decimal).\n");
		return 1;
	}
	unsigned char r, g, b;
	r = (unsigned char) atoi(argv[1]);
	g = (unsigned char) atoi(argv[2]);
	b = (unsigned char) atoi(argv[3]);

	unsigned char data[] = {0x00, 0x07, 0x0A, g, r, b, 0x00, 0x00, 0x00};
	return encode_and_send_report(dev, data, FEATURE_REPORT);
}

command_t get_command_setledcolor() {
	command_t result = {
		"setledcolor",
		"Set RGB led color. Requires 3 numbers as arguments (R, G and B values in decimal).",
		&execute
	};
	return result;
}
