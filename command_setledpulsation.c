#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <hidapi/hidapi.h>
#include "command.h"
#include "encoding.h"

static int execute(int argc, char** argv, hid_device* dev) {
	if (argc != 2) {
		fwprintf(stderr, L"Please specify 1 argument.\n");
		return 1;
	}

	const char* alternatives[] = {"steady", "slow", "middle", "fast"};
	unsigned char value = 0;
	for(int i = 0; i < 4; i++) {
		if (strcmp(alternatives[i], argv[1]) == 0) {
			value = i + 1;
			break;
		}
	}
	if (value == 0) {
		value = (char) atoi(argv[1]);
	}
	if (value < 1 || value > 4) {
		fwprintf(stderr, L"Unrecognized argument \"%s\".\n", argv[1]);
		return 1;
	}

	wprintf(L"Setting led pulsation effect to value %d (%s).\n", value, alternatives[value - 1]);

	unsigned char data[] = {0, 0x7, 0xB, value, 0, 0, 0, 0, 0};
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

struct command get_command_setledpulsation() {
	struct command result = {
		"setledpulsation",
		"Set RGB led pulsation effect. Requires one string or integer argument: \"steady\" or 1, \"slow\" or 2, \"middle\" or 3, \"fast\" or 4.",
		&execute
	};
	return result;
}
