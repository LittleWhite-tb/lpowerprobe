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

#ifndef H_BARRIER
#define H_BARRIER

#include <pthread.h>

/**
 * Simulation of pthread_barrier_t using pthread condition
 */
typedef struct Barrier
{
   pthread_mutex_t mutex;
   pthread_cond_t cond;
   
   unsigned int count;  /*!< Actual barrier counter */
   unsigned int nbToWait; /*!< Number of thread to wait at barrier */
}Barrier;

/**
 * Creates the barrier with a specific number of thread to wait
 *
 * \param b the barrier to create/initialized
 * \param count the nuùber of thread to wait
 */
int barrier_create(Barrier* b, unsigned int count);

/**
 * Wait at the barrier
 * When the number of thread waiting at this barrier until the number of
 * waiting thread is \a count,
 *
 * \param b the barrier to use
 */
int barrier_wait(Barrier* b);

/**
 * Destroys a barrier
 *
 * \param b the barrier to destroy
 */
int barrier_destroy(Barrier* b);

#endif
