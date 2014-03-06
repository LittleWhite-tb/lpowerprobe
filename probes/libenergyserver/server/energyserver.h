/*
lPowerProbe - A light benchmark tool oriented for energy probing
              heavely based on likwid
Copyright (C) 2014 Universite de Versailles

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

#ifndef H_ENERGYSERVER
#define H_ENERGYSERVER

#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

/** Name of the shared memory file */
#define ESRV_SHM_NAME "/LPP_NRGserver"

/** Maximal number of MSR probes allowed */
#define ESRV_MAX_NB_DEVICES 4

/** content of the shared memory */
typedef struct {
   sem_t measure;    /** post to request an energy update */
   sem_t read;       /** post when request served */
   unsigned int nb_devices;   /** number of MSR probes */
   double energy[ESRV_MAX_NB_DEVICES];   /** energy per probe */
} server_data;

#endif

