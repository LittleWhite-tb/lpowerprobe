#
# lPowerProbe - A light benchmark tool oriented for energy probing
#               heavely based on likwid
# Copyright (C) 2013 Universite de Versailles
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

PREFIX=/usr
BINDIR=$(PREFIX)/bin/
LIBDIR=$(PREFIX)/lib/
DATADIR=$(PREFIX)/share/
VERSION=v2.0 ($(shell git rev-list HEAD --count) ; $(shell git rev-list HEAD | head -n 1))

CC=gcc
CXX=g++
LD=$(CXX)

CFLAGS+=-Wextra -Wall -g
CXXFLAGS+=-Wextra -Wall -g 

# Allows having these in sub-makefiles
export CC
export CXX
export LD
export CFLAGS
export CXXFLAGS
export LDFLAGS
export PREFIX
export DATA_DIR
export VERSION

# local variable, if I change the variable, the changes will be forwarded in sub-makefiles
LOCCFLAGS=${CFLAGS} -O2 -DDATA_DIR="\"$(DATADIR)\"" -DINSTALL_DIR="\"$(PREFIX)\"" -DVERSION="\"$(VERSION)"\"
LOCCXXFLAGS=${CXXFLAGS} -O2 -DDATA_DIR="\"$(DATADIR)\"" -DINSTALL_DIR="\"$(PREFIX)\"" -DVERSION="\"$(VERSION)"\"
LOCLDFLAGS=${LDFLAGS} -ldl -lpthread -lrt

LPP_VERSION=2.0

SRC=$(wildcard src/*.cpp)
OBJ=$(SRC:.cpp=.o)

EXEC=lPowerProbe


.PHONY: clean test doc all install uninstall extra distclean

all: $(EXEC) test extra

install: all
	mkdir -p $(BINDIR)
	mkdir -p $(LIBDIR)
	mkdir -p $(DATADIR)
	cp $(EXEC) $(BINDIR)
	#setcap CAP_SYS_RAWIO+eip $(BINDIR)/$(EXEC)
	mkdir -p $(LIBDIR)/lPowerProbe/
	cp probes/*/*.so $(LIBDIR)/lPowerProbe/
	mkdir -p $(DATADIR)/lPowerProbe/
	cp empty/empty empty/empty.s $(DATADIR)/lPowerProbe/

uninstall:
	rm -f $(BINDIR)/$(EXEC)
	rm -rf $(LIBDIR)/lPowerProbe
	rm -rf $(DATADIR)/lPowerProbe

$(EXEC): $(OBJ)
	echo "LOLkmoOL"
	$(LD) $(OBJ) -o $@ $(LOCLDFLAGS) 

%.o: %.cpp *.hpp
	$(CXX) $(LOCCXXFLAGS) -c $< -o $@

%.o: %.cpp *.h
	$(CXX) $(LOCCXXFLAGS) -c $< -o $@

%.o: %.c *.h
	$(CC) $(LOCCFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(LOCCXXFLAGS) -c $< -o $@

test:
	make -C empty/

extra:
	make LPP_API_VERSION=$(LPP_VERSION) -C probes/libenergymic
	make LPP_API_VERSION=$(LPP_VERSION) -C probes/libenergymsr
	make LPP_API_VERSION=$(LPP_VERSION) -C probes/libwallclock 
	make LPP_API_VERSION=$(LPP_VERSION) -C probes/libenergyyoko 
	make LPP_API_VERSION=$(LPP_VERSION) -C probes/pfmcounters 
	make LPP_API_VERSION=$(LPP_VERSION) -C probes/libtimer 
	make -C scripts/MPI_preload

doc:
	doxygen doc/Doxyfile.in

clean:
	rm -f $(OBJ)

distclean: clean
	make -C ./empty/ clean
	make -C probes/libenergymic clean
	make -C probes/libwallclock clean
	make -C probes/libenergymsr clean
	make -C probes/libenergyyoko distclean
	make -C probes/pfmcounters clean
	make -C probes/libtimer clean
	make -C scripts/MPI_preload clean
	rm -f $(EXEC)
