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
DATA_DIR=$(PREFIX)/share/
VERSION=v2.0 ($(shell git rev-list HEAD --count) ; $(shell git rev-list HEAD | head -n 1))

CC=gcc
CXX=g++
LD=$(CXX)

CFLAGS=-Wextra -Wall -g -O2 -DDATA_DIR="\"$(DATA_DIR)\"" -DINSTALL_DIR="\"$(PREFIX)\"" -DVERSION="\"$(VERSION)"\"
CXXFLAGS=-Wextra -Wall -g -O2 -DDATA_DIR="\"$(DATA_DIR)\"" -DINSTALL_DIR="\"$(PREFIX)\"" -DVERSION="\"$(VERSION)"\"
LDFLAGS=-ldl -lpthread -lrt

LPP_VERSION=2.0

SRC=$(wildcard src/*.cpp)
OBJ=$(SRC:.cpp=.o)

EXEC=lPowerProbe


.PHONY: clean test doc all install uninstall extra distclean

all: $(EXEC) test extra

install: all
	cp $(EXEC) $(INSTALL_DIR)/bin
	-setcap CAP_SYS_RAWIO+eip $(INSTALL_DIR)/bin/$(EXEC)
	mkdir -p $(INSTALL_DIR)/lib/lPowerProbe/
	cp probes/*/*.so $(INSTALL_DIR)/lib/lPowerProbe/
	mkdir -p $(INSTALL_DIR)/share/lPowerProbe/
	cp empty/empty empty/empty.s $(INSTALL_DIR)/share/lPowerProbe/

uninstall:
	rm -f $(INSTALL_DIR)/bin/$(EXEC)
	rm -rf $(INSTALL_DIR)/lib/lPowerProbe
	rm -rf $(INSTALL_DIR)/share/lPowerProbe

$(EXEC): $(OBJ)
	$(LD) $(OBJ) -o $@ $(LDFLAGS) 

%.o: %.cpp *.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp *.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c *.h
	$(CC) $(CFLAGS) -c $< -o $@

test:
	make -C empty/ CC="$(CC)"

extra:
	make LPP_API_VERSION=$(LPP_VERSION) -C probes/libenergymic CC="$(CC)"
	make LPP_API_VERSION=$(LPP_VERSION) -C probes/libenergymsr CC="$(CC)"
	make LPP_API_VERSION=$(LPP_VERSION) -C probes/libwallclock CC="$(CC)"
	make LPP_API_VERSION=$(LPP_VERSION) -C probes/libenergyyoko CC="$(CC)"
	make LPP_API_VERSION=$(LPP_VERSION) -C probes/pfmcounters CC="$(CC)"
	make LPP_API_VERSION=$(LPP_VERSION) -C probes/libtimer CC="$(CC)"
	make -C scripts/MPI_preload

doc:
	doxygen lPowerProbe.doxy

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
