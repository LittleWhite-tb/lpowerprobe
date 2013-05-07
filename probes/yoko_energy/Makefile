CC=gcc
CFLAGS=-O3 -g -Wall -Wextra -fPIC #-DNDEBUG
LDFLAGS=-shared -lm

all: libyoko_energy.so

libyoko_energy.so: libyoko_energy.o yokosrv.o
	$(CC) $^ $(LDFLAGS) -o $@

%.o: %.c %.h
	$(CC) $< -c $(CFLAGS) -o $@

%.o: %.c
	$(CC) $< -c $(CFLAGS) -o $@

.PHONY: clean mrproper

mrproper: clean
	rm -f libyoko_energy.so

clean:
	rm -f *.o
