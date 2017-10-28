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

	const char* alternatives[] = {"600", "1200", "2000", "3000", "4800", "6400", "8200"};
	int value = get_index_or_atoi(argv[1], alternatives, 7, "Setting sensor DPI to value");
	if (value == -1) {
		return 1;
	}

	unsigned char data[] = {0, 0x7, 0x9, (unsigned char) value, 0, 0, 0, 0, 0};
	return encode_and_send_feature_report(dev, data);
}

struct command get_command_setdpi() {
	struct command result = {
		"setdpi",
		"Set sensor DPI. Requires one argument: \"600\" or 1, \"1200\" or 2, \"2000\" or 3, \"3000\" or 4, \"4800\" or 5, \"6400\" or 7, \"8200\" or 8.",
		&execute
	};
	return result;
}
