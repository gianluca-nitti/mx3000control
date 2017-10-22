#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <hidapi/hidapi.h>
#include "command.h"

int find_command(const char* cmd, const struct command* commands, const int num_commands) {
	for(int i = 0; i < num_commands; i++) {
		if (strcmp(cmd, commands[i].cmd) == 0) {
			return i;
		}
	}
	fwprintf(stderr, L"Unknown command \"%s\".\n", cmd);
	exit(1);
}

int main(int argc, char** argv) {
	const struct command commands[] = {get_command_raw(), get_command_setrgbled()};
	const int num_commands = 2;

	if (argc < 2) {
		fwprintf(stderr, L"Please specify a command as 1st argument.\n"); //TODO: implement help command
		return 1;
	}

	int cmd_index = find_command(argv[1], commands, num_commands);
	//commands[cmd_index].execute(argc - 1, argv + 1, NULL); //TODO remove

	int status = hid_init();
	if (status != 0) {
		wprintf(L"Failed to initialize HID (hid_init() returned %d)\n", status);
		return 1;
	}

	struct hid_device_info* device_list = hid_enumerate(0x04d9, 0xa067);
	struct hid_device_info* dev_info = device_list;
	while (dev_info != NULL) {
		wprintf(L"Found device 0x%X 0x%X Serial: \"%S\" Manufacturer: \"%S\" Device: \"%S\"\n",
				dev_info->vendor_id, dev_info->product_id,
				dev_info->serial_number, dev_info->manufacturer_string,
				dev_info->product_string);

		hid_device* dev = hid_open_path(dev_info->path);
		if (dev == NULL) {
			wprintf(L"Failed to open device at path %s.\n", dev_info->path);
		} else {
			commands[cmd_index].execute(argc - 1, argv + 1, dev);
			hid_close(dev);
		}

		dev_info = dev_info->next;
	}
	hid_free_enumeration(device_list);
	hid_exit();
	return 0;
}
