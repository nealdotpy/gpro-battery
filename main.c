#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wchar.h>
#include <string.h>
#include "hidapi.h"

#define MAX_STR 255

#define LOGI_HID_VID 0x046d
#define LOGI_HID_PID 0x4079

#define PRINT_DEVS 0

int main(int argc, char* argv[]) {
	printf("Attempting to run gpro-battery...\n");
	
	int res;
	unsigned char buf[64];
	wchar_t wstr[MAX_STR];
	hid_device* handle;
	int i;

	if (hid_init()) {
		fprintf(stderr, "hid_init() failed!\n");
		return -1;
	}


	if (PRINT_DEVS) {

		struct hid_device_info *devs, *cur_dev;

		devs = hid_enumerate(LOGI_HID_VID, LOGI_HID_PID);
		cur_dev = devs;
		while (cur_dev) {
			printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
			printf("\n");
			printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
			printf("  Product:      %ls\n", cur_dev->product_string);
			printf("  Release:      %hx\n", cur_dev->release_number);
			printf("  Interface:    %d\n",  cur_dev->interface_number);
			printf("  Usage (page): 0x%hx (0x%hx)\n", cur_dev->usage, cur_dev->usage_page);
			printf("\n");
			cur_dev = cur_dev->next;
		}
		hid_free_enumeration(devs);
	}





	printf("Testing...\n");

	// set up command buffer.
	memset(buf,0x00,sizeof(buf));
	buf[0] = 0x01;
	buf[1] = 0x81;

	// Open the device using the VID, PID, Serial No.
	handle = hid_open(LOGI_HID_VID, LOGI_HID_PID, NULL);
	// handle = hid_open_path("/dev/hidraw0");
	if (!handle) {
		fprintf(stderr, "unable to open device\n");
 		return 1;
	}

	// manufacturer
	wstr[0] = 0x0000;
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read manufacturer string.\n");
	else
		printf("Manufacturer String: %ls\n", wstr);

	// product
	wstr[0] = 0x0000;
	res = hid_get_product_string(handle, wstr, MAX_STR);
	if (res < 0)
		printf("Unable to read product string.\n");
	else
		printf("Product String: %ls\n", wstr);

	// Set the hid_read() function to be non-blocking.
	hid_set_nonblocking(handle, 1);
	// Try to read from the device. There should be no
	// data here, but execution should not block.
	if (hid_read(handle, buf, 8) < 0) {
		fprintf(stderr, "Failed at reading nothing!\n");
	}

	printf("Done with testing...\n");

	/* 
	FEATURE REPORT
	https://drive.google.com/file/d/0BxbRzx7vEV7eNDBheWY0UHM5dEU/view
			from https://discourse.wicg.io/t/human-interface-device-hid-api/3070/3

	HID++ Message Format:
	[reportID] [device index] [sub ID] [		parameters		]
	
	report ID : 	short message (7B) use report ID 0x10
					long message (20B) use report ID 0x11

	device index :	receiver always uses device index 0xff

	sub ID :		0x00 - 0x7f are HID++ Reports and Notifications
					0x80 - 0xff are register access



	
	*/
	buf[0] = 0x10; // report ID
	buf[1] = 0xff; // device index
	buf[2] = 0x07; // sub ID -> Battery milage 0x0d (0x07 is the other one).
	buf[3] = 0x00;
	buf[4] = 0x00;
	buf[5] = 0x00;
	buf[6] = 0x00;
	// res = 
	if (hid_send_feature_report(handle, buf, 8) < 0) {
		fprintf(stderr, "Unable to send a feature report.\n");
		fprintf(stderr, "  error: %ls\n", hid_error(handle));
	} else {
		printf("Sent feature report!!!\n");
	}

	memset(buf,0,sizeof(buf));

	// Read a Feature Report from the device
	buf[0] = 0x10;
	// buf[1] = 0xff; // device index
	// buf[2] = 0x0d; // sub ID -> Battery milage 0x0d (0x07 is the other one).
	res = hid_get_feature_report(handle, buf, sizeof(buf));
	if (res < 0) {
		fprintf(stderr, "Unable to get a feature report.\n");
		fprintf(stderr, "  error: %ls\n", hid_error(handle));
	} else {
		// Print out the returned buffer.
		printf("Feature Report\n   ");
		for (i = 0; i < res; i++)
			printf("%02hhx ", buf[i]);
		printf("\n");
	}

	memset(buf,0,sizeof(buf));



	// close device and finalize lib
	hid_close(handle);
	res = hid_exit();

	return 0;

}