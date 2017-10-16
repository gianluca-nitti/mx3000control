#include <stdlib.h>
#include <stdio.h>
#include <hidapi/hidapi.h>

int main(int argc, char** argv) {
	if (argc != 9) {
		printf("Need 8 arguments (8 bytes of feature report to send in hex format)");
		return 1;
	}
	/* send feature report specified on command line (8 bytes, 1 for each argument, in hex) */
	unsigned char data[9];
	data[0] = 0;
	for (int i = 1; i < 9; i++) {
		data[i] = (char) strtol(argv[i], NULL, 16);
	}
	wprintf(L"Feature report to send: %X %X %X %X %X %X %X %X %X\n", data[0], data[1],
			data[2], data[3], data[4], data[5], data[6], data[7], data[8]);
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
			status = hid_send_feature_report(dev, data, 9);
			if (status == -1) {
				wprintf(L"Failed to send feature report: \"%S\".\n", hid_error(dev));
			} else {
				wprintf(L"Feature report sent.\n");
			}
			hid_close(dev);
		}

		dev_info = dev_info->next;
	}
	hid_free_enumeration(device_list);
	hid_exit();
	return 0;
}
