override CFLAGS:=$(CFLAGS) -O3 -fPIC -Wall -Wextra
override LDFLAGS:=$(LDFLAGS) -shared -ldl
MPICC=mpicc

all:liblppMPInotifier.so
	

.PHONY: all clean

liblppMPInotifier.so: liblppMPInotifier.c
	@if [ -x "`which $(MPICC)`" ] ; \
	then \
		${MPICC} ${CFLAGS} $< -o $@ ${LDFLAGS} ;\
	else \
		echo "INFO - liblppMPInotifier.so - You don't have $(MPICC), leaving" ;\
	fi

clean:
	rm -f liblppMPInotifier.so
