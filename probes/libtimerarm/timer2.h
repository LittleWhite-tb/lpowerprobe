/*
Copyright (C) 2015 Exascale Research Center

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef H_TIMER
#define H_TIMER

#ifndef LPP_API_VERSION
   #error LPP_API_VERSION undefined, never call this makefile directly!
#endif

/**
 * API version used.
 */
const unsigned int version = LPP_API_VERSION;

/**
 * Human readable label for the results.
 */
const char *label = "Cycles (per thread)";

/**
 * Period at which the update function has to be called. If 0, the function is
 * never called. The period is expressed in micro seconds (us).
 */
const unsigned int period = 0;

/**
 * Returns the number of devices this library has detected.
 *
 * @return The number of devices considered by this lib.
 */
extern unsigned int nbDevices(void *data);

/**
 * Returns the number of channels per device to use.
 *
 * @return The number of channels to associate to every device. It is the number
 * of different result values per device in the return value of stop.
 */
extern unsigned int nbChannels(void *data);

extern void *init (void);
extern void fini (void *data);
extern void start (void *data);
extern double* stop (void *data);
extern void update (void *data);
 

#endif
