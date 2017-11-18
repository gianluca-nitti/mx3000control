#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <hidapi/hidapi.h>
#include "../command.h"
#include "../util.h"
#include "../encoding.h"

static int execute(int argc, char** argv, hid_device* dev) {
	if (argc != 9) {
		fwprintf(stderr, L"Please specify 8 arguments (8 bytes in hex format).\n");
		return 1;
	}
	unsigned char data[9];
	data[0] = 0;
	for (int i = 1; i < 9; i++) {
		data[i] = (char) strtol(argv[i], NULL, 16);
	}
	encode(data);
	wprintf(L"%02X %02X %02X %02X %02X %02X %02X %02X %02X\n", data[0], data[1],
			data[2], data[3], data[4], data[5], data[6], data[7], data[8]);
	return 0;
}

struct command get_command_encode() {
	struct command result = {
		"encode",
		"Encodes the specified hex command. Doesn't actually send anything to the mouse, useful for debug/development purposes only.",
		&execute
	};
	return result;
}
