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
	unsigned char buf[65];
	wchar_t wstr[MAX_STR];
	hid_device* handle;
	int i;

	res = hid_init();

	// Open the device using the VID, PID, Serial No.
	handle = hid_open(0x046d, 0xc539, NULL);
	if (!handle) {
		printf("unable to open device\n");
 		return 1;
	}

	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	wprintf(L"Manufacturer String: %s\n", wstr);

	// close device and finalize lib
	hid_close(handle);
	res = hid_exit();

	return 0;

}