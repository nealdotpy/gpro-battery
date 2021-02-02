#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wchar.h>
#include <string.h>
#include "hidapi.h"

#define MAX_STR 255

int main(int argc, char* argv[]) {
	printf("Attempting to run gpro-battery...\n");
	
	int res;
	unsigned char buf[256];
	wchar_t wstr[MAX_STR];
	hid_device* handle;
	int i;

	if (hid_init())
		return -1;

	// set up command buffer.
	memset(buf,0x00,sizeof(buf));
	buf[0] = 0x01;
	buf[1] = 0x81;

	// Open the device using the VID, PID, Serial No.
	handle = hid_open(0x046d, 0xc539, NULL);
	if (!handle) {
		printf("unable to open device\n");
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
	res = hid_read(handle, buf, 17);



	// FEATURE REPORT
	buf[0] = 0x2;
	buf[1] = 0xa0;
	buf[2] = 0x0a;
	buf[3] = 0x00;
	buf[4] = 0x00;
	res = hid_send_feature_report(handle, buf, 17);
	if (res < 0) {
		printf("Unable to send a feature report.\n");
	}

	memset(buf,0,sizeof(buf));

	// Read a Feature Report from the device
	buf[0] = 0x2;
	res = hid_get_feature_report(handle, buf, sizeof(buf));
	if (res < 0) {
		printf("Unable to get a feature report.\n");
		printf("error: %ls\n", hid_error(handle));
	}
	else {
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