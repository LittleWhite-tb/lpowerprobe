/*
 * lPowerProbe - A light benchmark tool oriented for energy probing
 *               heavely based on likwid
 * Copyright (C) 2013-2014 Universite de Versailles
 * Copyright (C) 2012 Exascale Research Center
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <sys/time.h>
#include <stdlib.h>
#include "libwallclock.h"

extern unsigned int nbDevices(void *data) {
   (void) data;
   return 1;
}

extern unsigned int nbChannels(void *data) {
   (void) data;
   return 1;
}

extern void *init (void) {
   return malloc(sizeof(double));
}

extern void fini (void *data) {  
   free(data);
}

static double getTimeStamp() {
   struct timeval tv;

	gettimeofday(&tv, NULL);
	return tv.tv_usec + tv.tv_sec * 1000000.;
}

extern void start (void *data) {
   *((double *) data) = -getTimeStamp();
}

extern double *stop (void *data) {
   *((double *) data) += getTimeStamp();
   return data;
}

extern void update (void *data) {
   (void) data;
}

