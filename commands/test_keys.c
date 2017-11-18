#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <hidapi/hidapi.h>
#include "../command.h"
#include "../util.h"

static int execute(int argc, char** argv, hid_device* dev) {

	unsigned char data1[] = {0, 0x07, 0x3A, 0x2B, 0xB7, 0xCF, 0x33, 0xA7, 0xB2};
	hid_send_feature_report(dev, data1, 9);
	usleep(10000);
	unsigned char data2[] = {0, 0x07, 0x3A, 0x2B, 0xB7, 0xC7, 0x33, 0xA7, 0xB2};
	hid_send_feature_report(dev, data2, 9);
	usleep(10000);
	unsigned char data4[] = {0, 0x07, 0x3A, 0x2D, 0xB7, 0xD7, 0x33, 0xA7, 0xB2};
	hid_send_feature_report(dev, data4, 9);

	usleep(10000);
	unsigned char dataA[] = {0, 0, 0x3A, 0x23, 0xB7, 0x37, 0x33, 0x2F, 0xB2};
	hid_write(dev, dataA, 9);
	usleep(10000);
	unsigned char dataB[] = {0, 0, 0x3A, 0x23, 0xB7, 0x47, 0x33, 0x47, 0xB2};
	hid_write(dev, dataB, 9);
	usleep(10000);
	unsigned char dataC[] = {0, 0, 0x3A, 0x23, 0xB7, 0x4F, 0x33, 0xA7, 0xB2};
	hid_write(dev, dataC, 9);
	usleep(10000);
	unsigned char dataD[] = {0, 0, 0xC2, 0x23, 0xB7, 0xB7, 0x33, 0xAF, 0xB5};
	hid_write(dev, dataD, 9);
	usleep(10000);
	unsigned char dataE[] = {0, 0x71, 0xCA, 0x23, 0xB7, 0xC7, 0x33, 0xAF, 0xB5};
	hid_write(dev, dataE, 9);
	usleep(10000);
	unsigned char dataF[] = {0, 0, 0x3A, 0x23, 0xB7, 0xB7, 0x33, 0xA7, 0xB2};
	hid_write(dev, dataF, 9);
	usleep(10000);
	unsigned char dataG[] = {0, 0, 0x3A, 0x23, 0xB7, 0xB7, 0x33, 0xA7, 0xB2};
	hid_write(dev, dataG, 9);
	usleep(10000);
	unsigned char dataH[] = {0, 0, 0x3A, 0x23, 0xB7, 0xB7, 0x33, 0xA7, 0xB2};
	hid_write(dev, dataH, 9);

	usleep(10000);
	unsigned char data5[] = {0, 0x07, 0x6A, 0x2B, 0xB7, 0xDF, 0x33, 0xA7, 0xB2};
	hid_send_feature_report(dev, data5, 9);

	for(int i = 0; i < 192; i++) {
		usleep(10000);
		unsigned char data[] = {0, 0, 0x3A, 0x23, 0xB7, 0xB7, 0x33, 0xA7, 0xB2};
		hid_write(dev, data, 9);
	}

	return 0;
}

struct command get_command_test_keys() {
	struct command result = {
		"test_keys",
		"",
		&execute
	};
	return result;
}
