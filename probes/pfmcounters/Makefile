override CFLAGS:=$(CFLAGS) -O3 -fPIC -shared -DLPP_API_VERSION=$(LPP_API_VERSION)

all: pfmcounters.so

pfmcounters.so: pfmcounters.c pfmcounters.h 
	@if [ -n "$(/sbin/ldconfig -p | grep pfm)" ] ; then \
		$(CC) $(CFLAGS) pfmcounters.c inih/ini.c -Iinih -o $@  -lpfm ; \
	fi

clean:
	@if [ -n "$(/sbin/ldconfig -p | grep pfm)" ] ; then \
		rm -f pfmcounters.so ; \
	fi

install: all
	cp pfmcounters.so /usr/lib/libpfmcounters.so
	cp pfmcounters.h /usr/include/libpfmcounters.h
	
uninstall:
	rm -f /usr/lib/libpfmcounters.so
	rl -f /usr/include/libpfmcounters.h
