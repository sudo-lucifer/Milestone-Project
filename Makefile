CC = gcc
CFLAGS = -O -Wall -m32 -g

all: icsh

icsh: icsh.c executeCommand.c executeCommand.h
	$(CC) $(CFLAGS) -o icsh icsh.c executeCommand.c

clean:
	rm -f *.o icsh *~
