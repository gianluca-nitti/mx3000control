#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <hidapi/hidapi.h>
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

int get_index(char* str, const char** options, int n_options) {
	for(int i = 0; i < n_options; i++) {
		if (strcmp(str, options[i]) == 0) {
			return i;
		}
	}
	return -1;
}

int get_index_or_atoi(char* str, const char** options, int n_options, char* message_prefix) {
	int index = get_index(str, options, n_options) + 1;
	if (index == 0) {
		index = atoi(str);
	}
	if (index < 1 || index > n_options) {
		fwprintf(stderr, L"Unrecognized value \"%s\".\n", str);
		return -1;
	}
	wprintf(L"%s %d (%s).\n", message_prefix, index, options[index - 1]);
	return index;
}
