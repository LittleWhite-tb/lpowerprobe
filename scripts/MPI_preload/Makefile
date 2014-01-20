CFLAGS+=-O3 -fPIC -Wall -Wextra
LDFLAGS+=-shared -ldl
CC=mpicc

all:liblppMPInotifier.so
	

.PHONY: all clean

liblppMPInotifier.so: liblppMPInotifier.c
	@echo "${CC} ${CFLAGS} $< -o $@ ${LDFLAGS}"
	@${CC} --version >/dev/null
	@if [ $$? -eq 0 ] ; \
	then \
		${CC} ${CFLAGS} $< -o $@ ${LDFLAGS} ;\
	else \
		@echo "INFO - liblppMPInotifier.so - You don't have mpicc, leaving" ;\
	fi

clean:
	rm -f liblppMPInotifier.so
