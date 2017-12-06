#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <hidapi.h>
#include "mx3000.h"
#include "command.h"

int find_command(const char* cmd, const command_t* commands, const int num_commands) {
	for(int i = 0; i < num_commands; i++) {
		if (strcmp(cmd, commands[i].cmd) == 0) {
			return i;
		}
	}
	fwprintf(stderr, L"Unknown command \"%s\".\n", cmd);
	exit(1);
}

int main(int argc, char** argv) {
	const command_t commands[] = {get_command_help(), get_command_encode(), get_command_raw(),
		get_command_setledcolor(), get_command_setledpulsation(), get_command_setledintensity(),
		get_command_setdpi(), get_command_setsensivity(), get_command_setbuttons()};
	const int num_commands = sizeof(commands) / sizeof(commands[0]);

	if (argc < 2) {
		fwprintf(stderr, L"Please specify a command as 1st argument.\nUse \"%s help\" to show a list of the available commands.\n", argv[0]);
		return 1;
	}

	init_help(commands, num_commands);

	int cmd_index = find_command(argv[1], commands, num_commands);

	if (cmd_index < 2) { // "help" and "encode" commands does not need the handle to a device
		return commands[cmd_index].execute(argc - 1, argv + 1, NULL);
	}

	int status = hid_init();
	if (status != 0) {
		wprintf(L"Failed to initialize HID (hid_init() returned %d)\n", status);
		return 1;
	}

	struct hid_device_info* device_list = hid_enumerate(VENDOR_ID, PRODUCT_ID);
	struct hid_device_info* dev_info = device_list;
	int result = -1;
	while (dev_info != NULL) {
		wprintf(L"Found device 0x%X 0x%X Serial: \"%S\" Manufacturer: \"%S\" Device: \"%S\"\n",
				dev_info->vendor_id, dev_info->product_id,
				dev_info->serial_number, dev_info->manufacturer_string,
				dev_info->product_string);

		hid_device* dev = hid_open_path(dev_info->path);
		if (dev == NULL) {
			fwprintf(stderr, L"Failed to open device at path %s.\n", dev_info->path);
		} else {
			result = commands[cmd_index].execute(argc - 1, argv + 1, dev);
			hid_close(dev);
			break;
		}

		dev_info = dev_info->next;
	}
	hid_free_enumeration(device_list);
	hid_exit();
	if (result == -1) {
		fwprintf(stderr, L"No devices found.\n");
		result = 2;
	}
	return result;
}
