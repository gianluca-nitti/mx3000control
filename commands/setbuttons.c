#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <hidapi.h>
#include "../mx3000.h"
#include "../command.h"
#include "../encoding.h"
#include "../keymap.h"

static char* defaults[] = {"click", "menu", "middle-button", "backward", "forward", "dpi-up", "dpi-down", "led-color-switch"};

static void configure_keys(hid_device* dev, key_config_t config1, key_config_t config2) {
	unsigned char data[] = {0x00, config1.byteA, config1.byteB, config1.byteC, config1.byteD,
		config2.byteA, config2.byteB, config2.byteC, config2.byteD};
	encode(data);
	hid_write(dev, data, 9);
	usleep(DELAY);
}

static key_config_t get_key_config(char* keyName) {
	if (strncmp(keyName, "key-", 3) == 0) {
		unsigned char scancode = (unsigned char) strtol(keyName + 4, NULL, 16);
		key_config_t result = {keyName, 0x00, scancode, 0x00, 0x00};
		return result;
	}
	for (int i = 0; i < numKeys; i++) {
		if (strcmp(keyName, keys[i].name) == 0) {
			return keys[i];
		}
	}
	fwprintf(stderr, L"Warning: unknown key \"%s\", setting to \"%s\" instead\n", keyName, keys[0].name);
	return keys[0];
}

static int execute(int argc, char** argv, hid_device* dev) {

	char** values;
	if (argc == 1) {
		values = defaults;
	} else if (argc == 9) {
		values = argv + 1;
	} else {
		fwprintf(stderr, L"Please specify 8 arguments (one string for each mouse button) or no arguments (to reset to defaults).\n");
		return 1;
	}

	// The following byte arrays are the pre-encoded version of the byte sequence in the comment on the previous line.
	// These values never change, so there is no reason to encode at runtime.
	// {0x00, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00}
	unsigned char data1[] = {0x00, 0x07, 0x3A, 0x2B, 0xB7, 0xCF, 0x33, 0xA7, 0xB2};
	hid_send_feature_report(dev, data1, 9);
	usleep(DELAY);
	// {0x00, 0x07, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00}
	unsigned char data2[] = {0x00, 0x07, 0x3A, 0x2B, 0xB7, 0xC7, 0x33, 0xA7, 0xB2};
	hid_send_feature_report(dev, data2, 9);
	usleep(DELAY);
	// {0x00, 0x07, 0x04, 0x01, 0x40, 0x00, 0x00, 0x00, 0x00}
	unsigned char data3[] = {0x00, 0x07, 0x3A, 0x2D, 0xB7, 0xD7, 0x33, 0xA7, 0xB2};
	hid_send_feature_report(dev, data3, 9);
	usleep(DELAY);

	configure_keys(dev, get_key_config(values[0]), get_key_config(values[1]));
	configure_keys(dev, get_key_config(values[2]), get_key_config(values[3]));
	configure_keys(dev, get_key_config(values[4]), get_key_config("disable"));
	configure_keys(dev, get_key_config("disable"), get_key_config(values[5]));
	configure_keys(dev, get_key_config(values[6]), get_key_config(values[7]));

	unsigned char data[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	encode(data);
	hid_write(dev, data, 9);
	usleep(DELAY);
	hid_write(dev, data, 9);
	usleep(DELAY);
	hid_write(dev, data, 9);

	usleep(DELAY);
	// {0x00, 0x07, 0x05, 0x01, 0x00, 0x06, 0x00, 0x00, 0x00}
	unsigned char data4[] = {0x00, 0x07, 0x6A, 0x2B, 0xB7, 0xDF, 0x33, 0xA7, 0xB2};
	hid_send_feature_report(dev, data4, 9);

	for(int i = 0; i < 192; i++) {
		usleep(DELAY);
		hid_write(dev, data, 9);
	}

	wprintf(L"Wrote button configuration.\n");

	return 0;
}

command_t get_command_setbuttons() {
	command_t result = {
		"setbuttons",
		"Configure mappings of the mouse buttons. Can be used with 8 string arguments - one for each button - or without any argument (resets to the default button configuration). You can assign to each button a mouse function, chosen from a list (run \"mx3000control help buttons\" to see the list of the available button functions) or a keyboard key, using a string in the format \"key-<scancode>\" where <scancode> is the scancode of the desired key, in hexadecimal.",
		&execute
	};
	return result;
}
