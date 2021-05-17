CC = gcc
CFLAGS = -O -Wall -m32 -g

all: icsh

icsh: icsh.c executeCommand.c executeCommand.h readInput.c readInput.h scriptMode.c scriptMode.h
	$(CC) $(CFLAGS) -o icsh icsh.c executeCommand.c readInput.c scriptMode.c

# for debugging script mode shell
script: scriptMode.c executeCommand.c executeCommand.h readInput.c readInput.h
	$(CC) $(CFLAGS) -o scriptMode scriptMode.c executeCommand.c readInput.c

clean:
	rm -f *.o icsh scriptMode *~
