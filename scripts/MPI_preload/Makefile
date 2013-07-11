CFLAGS=-O3 -fPIC -Wall -Wextra
LDFLAGS=-shared -ldl
CC=mpicc

all:liblppMPInotifier.so
	

.PHONY: all clean

liblppMPInotifier.so: liblppMPInotifier.c
		$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f liblppMPInotifier.so
