#
# lPowerProbe - A light benchmark tool oriented for energy probing
#               heavely based on likwid
# Copyright (C) 2014 Universite de Versailles
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

override CFLAGS:=$(CFLAGS) -O3 -g -Wall -Wextra -I ../../libenergymsr/ -DLPP_API_VERSION=$(LPP_API_VERSION)
override LDFLAGS:=$(LDFLAGS) -lm

.PHONY: clean all

all: energy_server

energy_server: energyserver.c ../../libenergymsr/libenergymsr.c energyserver.h
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f energy_server

