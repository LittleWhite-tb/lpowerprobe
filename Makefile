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

CC=gcc
CXX=g++
LD=g++

CFLAGS=-Wextra -Wall -g -O3
LDFLAGS=-ldl -lpthread -lrt

OBJ=src/main.o src/Runner.o src/Experimentation.o  src/Probe.o src/Options.o src/CPUUtils.o
EXEC=lPowerProbe

.PHONY: clean test libs doc

all: $(EXEC) test libs

$(EXEC): $(OBJ)
	$(LD) $(OBJ) -o $@ $(LDFLAGS) 

%.o: %.cpp *.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.cpp *.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c *.h
	$(CC) $(CFLAGS) -c $< -o $@

test:
	make -C empty/

libs:
	make -C probes/energy_snb_msr
	make -C probes/wallclock

doc:
	doxygen lPowerProbe.doxy

clean:
	make -C ./empty/ clean
	make -C probes/energy_snb_msr clean
	make -C probes/wallclock clean
	rm -rf $(EXEC) $(OBJ)
