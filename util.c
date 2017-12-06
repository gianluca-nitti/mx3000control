#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <hidapi.h>
#include "encoding.h"
#include "util.h"

int send_feature_report(hid_device* dev, unsigned char* data) {
	wprintf(L"Feature report to send: %02X %02X %02X %02X %02X %02X %02X %02X %02X\n", data[0], data[1],
			data[2], data[3], data[4], data[5], data[6], data[7], data[8]);

	int status = hid_send_feature_report(dev, data, 9);
	if (status == -1) {
		wprintf(L"Failed to send feature report: \"%S\".\n", hid_error(dev));
	} else {
		wprintf(L"Feature report sent.\n");
	}
	return status;
}

int encode_and_send_feature_report(hid_device* dev, unsigned char* data) {
	encode(data);
	return send_feature_report(dev, data);
}

int get_index(const char* str, const char** options, int n_options) {
	for(int i = 0; i < n_options; i++) {
		if (strcmp(str, options[i]) == 0) {
			return i;
		}
	}
	return -1;
}

int get_index_or_atoi(const char* str, const char** options, int n_options, const char* setting_name) {
	int index = get_index(str, options, n_options) + 1;
	if (index == 0) {
		index = atoi(str);
	}
	if (index < 1 || index > n_options) {
		fwprintf(stderr, L"Unrecognized value \"%s\".\n", str);
		return -1;
	}
	wprintf(L"Setting %s to value %d (%s).\n", setting_name, index, options[index - 1]);
	return index;
}

int execute_simple_command(int argc, char** argv, hid_device* dev, const char** options, int n_options, unsigned char command_byte, const char* setting_name) {
	if (argc != 2) {
		fwprintf(stderr, L"Please specify 1 argument.\n");
		return 1;
	}

	int value = get_index_or_atoi(argv[1], options, n_options, setting_name);
	if (value == -1) {
		return 1;
	}

	unsigned char data[] = {0x00, 0x07, command_byte, (unsigned char) value, 0x00, 0x00, 0x00, 0x00, 0x00};
	return encode_and_send_feature_report(dev, data);
}

