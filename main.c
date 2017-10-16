#include <stdio.h>
#include <hidapi/hidapi.h>

int main(void) {
	int status = hid_init();
	if (status != 0) {
		wprintf(L"Failed to initialize HID (hid_init() returned %d)\n", status);
		return 1;
	}
	struct hid_device_info* device_list = hid_enumerate(0x04d9, 0xa067);
	struct hid_device_info* dev_info = device_list;
	while(dev_info != NULL) {
		wprintf(L"Found device 0x%X 0x%X Serial: \"%S\" Manufacturer: \"%S\" Device: \"%S\"\n",
				dev_info->vendor_id, dev_info->product_id,
				dev_info->serial_number, dev_info->manufacturer_string,
				dev_info->product_string);

		hid_device* dev = hid_open_path(dev_info->path);
		if (dev == NULL) {
			wprintf(L"Failed to open device at path %s.\n", dev_info->path);
		} else {
			/* set RGB led to green */
			const unsigned char data[] = {0x00, 0x07, 0x52, 0x13, 0xb0, 0x07, 0x33, 0xa7, 0xba};
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
