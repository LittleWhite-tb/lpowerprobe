override CFLAGS:=$(CFLAGS) -O3 -fPIC -shared -DLPP_API_VERSION=$(LPP_API_VERSION)

all: pfmcounters.so

pfmcounters.so: pfmcounters.c pfmcounters.h 
	$(CC) $(CFLAGS) pfmcounters.c inih/ini.c -Iinih -o $@  -lpfm

clean:
	rm -f pfmcounters.so

install: all
	cp pfmcounters.so /usr/lib/libpfmcounters.so
	cp pfmcounters.h /usr/include/libpfmcounters.h
	
uninstall:
	rm -f /usr/lib/libpfmcounters.so
	rl -f /usr/include/libpfmcounters.h
