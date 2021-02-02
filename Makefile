gpro-battery: main.c
	gcc main.c -Wall -g -I /usr/include/hidapi/ -l hidapi-hidraw -o gpro-battery

clean:
	rm gpro-battery
