CC=icc
CFLAGS=-Wall -Wextra -shared -O3 -g -fPIC -lrt -mmic
LIBS=-lm -lpthread
LIBNAME=mic_energy.so
SRC=$(LIBNAME:.so=.c)

all: $(LIBNAME)

$(LIBNAME): $(SRC)
	$(CC) $< -o $@ $(CFLAGS) $(LIBS)

clean: 
	rm -rf $(LIBNAME)

