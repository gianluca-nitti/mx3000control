#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <hidapi.h>
#include "../command.h"
#include "../util.h"

static int execute(int argc, char** argv, hid_device* dev) {
	if (argc != 3) {
		fwprintf(stderr, L"Please specify 2 arguments.\n");
		return 1;
	}

	int x = atoi(argv[1]);
	int y = atoi(argv[2]);
	if (x < 1 || x > 10 || y < 1 || y > 10) {
		fwprintf(stderr, L"Values for X and Y sensivity must be in the 1 to 10 range (inclusive).\n");
		return 1;
	}
	unsigned char data[] = {0x00, 0x07, 0x12, (unsigned char) y, (unsigned char) x, 0x00, 0x00, 0x00, 0x00};
	return encode_and_send_report(dev, data, FEATURE_REPORT);
}

command_t get_command_setsensivity() {
	command_t result = {
		"setsensivity",
		"Set horizontal and vertical sensivity. Requires two integer arguments between 1 and 10, the first one is X and the second one is Y.",
		&execute
	};
	return result;
}
