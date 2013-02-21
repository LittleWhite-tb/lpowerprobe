CC=gcc
CFLAGS=-O3 -Wall -Wextra -g

.PHONY:
	clean

all: wallclock.so

wallclock.so: 
	$(CC) wallclock.c $(CFLAGS) -o $@ -fPIC -shared

clean:
	rm -rf ./wallclock.so
