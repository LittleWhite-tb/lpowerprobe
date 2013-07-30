CFLAGS=-O3 -fPIC -Wall -Wextra
LDFLAGS=-shared -ldl
CC=mpicc

all:liblppMPInotifier.so
	

.PHONY: all clean

liblppMPInotifier.so: liblppMPInotifier.c
ifeq ($(which mpicc >/dev/null),0)
		mpicc $(CFLAGS) $< -o $@ $(LDFLAGS)
else
		@echo "INFO - liblppMPInotifier.so - You don't have mpicc, leaving"
endif

clean:
	rm -f liblppMPInotifier.so
