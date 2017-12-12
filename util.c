#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <hidapi.h>
#include "mx3000.h"
#include "verbose.h"
#include "encoding.h"
#include "util.h"

extern verbose_mode verbosity;

static void print_report(char* prefix, report_type t, unsigned char* d) {
	wprintf(L"%s HID %s report: %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",
			prefix, t == FEATURE_REPORT ? "feature" : "output",
			d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7], d[8]);
}

int send_report(hid_device* dev, unsigned char* data, report_type t) {
	if (verbosity == PRINT_ALL_REPORTS) {
		print_report("Sending", t, data);
	}
	int status;
	if (t == FEATURE_REPORT) {
		status = hid_send_feature_report(dev, data, 9);
	} else {
		status = hid_write(dev, data, 9);
	}
	usleep(DELAY);
	if (status == -1) {
		fwprintf(stderr, L"Failed to send report: \"%S\".\n", hid_error(dev));
		return 1;
	} else {
		return 0;
	}
}

int encode_and_send_report(hid_device* dev, unsigned char* data, report_type t) {
	if (verbosity != SILENT) {
		print_report("Encoding", t, data);
	}
	encode(data);
	return send_report(dev, data, t);
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
	if (verbosity != SILENT) {
		wprintf(L"Setting %s to value %d (%s).\n", setting_name, index, options[index - 1]);
	}
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
	return encode_and_send_report(dev, data, FEATURE_REPORT);
}

