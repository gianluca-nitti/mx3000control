#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <hidapi/hidapi.h>
#include "../command.h"
#include "../util.h"
#include "../encoding.h"
#include "../keymap.h"

static void configure_keys(hid_device* dev, key_config_t config1, key_config_t config2) {
	unsigned char data[] = {0, config1.byteA, config1.byteB, config1.byteC, config1.byteD,
		config2.byteA, config2.byteB, config2.byteC, config2.byteD};
	encode(data);
	hid_write(dev, data, 9);
	usleep(10000);
}

static key_config_t get_key_config(char* keyName) {
	static int numKeys = sizeof(keys) / sizeof(keys[0]);
	for (int i = 0; i < numKeys; i++) {
		if (strcmp(keyName, keys[i].name) == 0) {
			return keys[i];
		}
	}
	fwprintf(stderr, L"Warning: unknown key \"%s\", setting to \"%s\" instead\n", keyName, keys[0].name);
	return keys[0];
}

static int execute(int argc, char** argv, hid_device* dev) {

	unsigned char data1[] = {0, 0x07, 0x3A, 0x2B, 0xB7, 0xCF, 0x33, 0xA7, 0xB2}; // 0x07 0x03 ...
	hid_send_feature_report(dev, data1, 9);
	usleep(10000);
	unsigned char data2[] = {0, 0x07, 0x3A, 0x2B, 0xB7, 0xC7, 0x33, 0xA7, 0xB2}; // 0x07 0x02 0x01 ...
	hid_send_feature_report(dev, data2, 9);
	usleep(10000);
	unsigned char data3[] = {0, 0x07, 0x3A, 0x2D, 0xB7, 0xD7, 0x33, 0xA7, 0xB2}; // 0x07 0x04 0x01 0x40 ...
	hid_send_feature_report(dev, data3, 9);
	usleep(10000);

	configure_keys(dev, get_key_config("click"), get_key_config("menu"));
	configure_keys(dev, get_key_config("middle-button"), get_key_config("backward"));
	configure_keys(dev, get_key_config("forward"), get_key_config("disable"));
	configure_keys(dev, get_key_config("disable"), get_key_config("dpi-up"));
	configure_keys(dev, get_key_config("dpi-down"), get_key_config("led-color-switch"));

	unsigned char data[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	encode(data);
	hid_write(dev, data, 9);
	usleep(10000);
	hid_write(dev, data, 9);
	usleep(10000);
	hid_write(dev, data, 9);

	usleep(10000);
	unsigned char data5[] = {0, 0x07, 0x6A, 0x2B, 0xB7, 0xDF, 0x33, 0xA7, 0xB2};
	hid_send_feature_report(dev, data5, 9);

	for(int i = 0; i < 192; i++) {
		usleep(10000);
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
