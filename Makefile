gpro-battery: main.c
	gcc main.c -Wall -g -I /usr/include/hidapi/ -o gpro-battery

clean:
	rm gpro-battery
