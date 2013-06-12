/*
 * lPowerProbe - A light benchmark tool oriented for energy probing
 *               heavely based on likwid
 * Copyright (C) 2013 Universite de Versailles
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

#ifndef H_LIBWALLCLOCK
#define H_LIBWALLCLOCK

#ifndef LPP_API_VERSION
#error LPP_API_VERSION undefined, never call this makefile directly!
#endif

/**
 * API version used.
 */
extern const unsigned int version = LPP_API_VERSION;

/**
 * Human readable label for the results.
 */
extern const char *label = "Time (us)";

/**
 * Period at which the update function has to be called. If 0, the function is
 * never called. The period is expressed in micro seconds (us).
 */
extern const unsigned int period = 0;

/**
 * Returns the number of devices this library has detected.
 *
 * @return The number of devices considered by this lib.
 */
extern unsigned int nbDevices();

/**
 * Returns the number of channels per device to use.
 *
 * @return The number of channels to associate to every device. It is the number
 * of different result values per device in the return value of stop.
 */
extern unsigned int nbChannels();

/**
 * Called first at initialization.
 *
 * @return A pointer to an internal library state. May be NULL.
 */
extern void *init (void);

/**
 * Called once at the end of the experiment.
 *
 * @param data A pointer to an internal library state as returned by init.
 * May be NULL.
 */
extern void fini (void *data);

/**
 * Called at the begining of the experiment, right after init or stop.
 * May be called several times during the experiment.
 *
 * @param data A pointer to an internal library state as returned by init.
 */
extern void start (void *data);

/**
 * Called a the end of the experiment, always after a call to start. May be
 * called several times during the experiment.
 *
 * @param data The internal library state as returned by init.
 *
 * @return The final result to be stored in the result file. The result is made
 * of one value per channel for every device. The results must be sorted by 
 * device and then by channel: 
 * device0/channel0, device0/channel1, ..., deviceN/channelM
 */
extern double *stop (void *data);

/**
 * Called periodically (rate defined by the period property). Can be used to
 * periodically update some internal states.
 *
 * @param data The internal library state as returned by init.
 */
extern void update (void *data);

#endif
