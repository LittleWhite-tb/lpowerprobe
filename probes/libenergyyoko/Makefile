CFLAGS+=-O3 -fPIC -DLPP_API_VERSION=$(LPP_API_VERSION) -DNDEBUG
LDFLAGS+=-shared -lm

all: libenergyyoko.so

libenergyyoko.so: libenergyyoko.o yokosrv.o
	$(CC) $^ $(LDFLAGS) -o $@

%.o: %.c %.h
	$(CC) $< -c $(CFLAGS) -o $@

%.o: %.c
	$(CC) $< -c $(CFLAGS) -o $@

.PHONY: clean distclean all

distclean: clean
	rm -f libenergyyoko.so

clean:
	rm -f *.o
