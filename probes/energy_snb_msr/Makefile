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

OBJECTS = obj
LIBRARIES = ./
INCLUDES = includes

ENERGY_LIB = $(LIBRARIES)/energy_msr_snb.so
ENERGY_OBJ = $(OBJECTS)/energy_msr_snb.o
MSR_OBJ = $(OBJECTS)/msr.o

.PHONY:
	clean

all: $(ENERGY_LIB)

$(ENERGY_LIB): $(MSR_OBJ) $(ENERGY_OBJ)
	mkdir -p $(LIBRARIES)
	$(CC) -o $(LIBRARIES)/energy_msr_snb.so -shared $(OBJECTS)/energy_msr_snb.o $(OBJECTS)/msr.o -lm -lpthread

$(ENERGY_OBJ): energy_msr_snb.c
	mkdir -p $(OBJECTS)
	$(CC) -o $(OBJECTS)/energy_msr_snb.o -c energy_msr_snb.c -fPIC

$(MSR_OBJ): $(INCLUDES)/msr.c
	mkdir -p $(OBJECTS)
	$(CC) -o $(OBJECTS)/msr.o -c $(INCLUDES)/msr.c -fPIC

clean: 
	rm -rf $(OBJECTS)/*.o $(LIBRARIES)/*.so
