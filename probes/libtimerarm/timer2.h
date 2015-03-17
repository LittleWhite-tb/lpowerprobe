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
 * @return The number of devices considered by this lib (here 1).
 */
extern unsigned int nbDevices(void *data);

/**
 * Returns the number of channels per device to use.
 * With libtimerarm, the number of channels is equivalent to the number of
 * cores available on the machine.
 *
 * @return equivalent to the number of core, since the counter is available on each core.
 */
extern unsigned int nbChannels(void *data);

/**
 * Allocates all the memory needed for the library to work. It will start
 * several measure threads (one per core).
 *
 * @return A pointer to an internal library state. May be NULL in case 
 * of errors.
 * @sa measureThread
 */
extern void *init (void);

/**
 * Cleanup everything
 *
 * @param data A pointer to the pointer returned by \a init()
 */
extern void fini (void *data);

/**
 * Start a measure by unlocking the measurement thread.
 *
 * @param data A pointer to the pointer returned by \a init()
 */
extern void start (void *data);

/**
 * Stops the measurement and gather the results.
 *
 * @param data A pointer to the pointer returned by \a init()
 * @return The final result to be stored in the result file. The result is made
 * of one value per channel for every device. The results must be sorted by 
 * device and then by channel: 
 * device0/core0, device0/core1, ..., device0/coreN
 */
extern double* stop (void *data);

/**
 * Unused in this probe
 * 
 * @param data A pointer to the pointer returned by \a init()
 */
extern void update (void *data);
 

#endif
