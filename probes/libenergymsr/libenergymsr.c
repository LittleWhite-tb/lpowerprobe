/*
lPowerProbe - A light benchmark tool oriented for energy probing
              heavely based on likwid
Copyright (C) 2013 Universite de Versailles
Copyright (C) 2011 Exascale Research Center

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


#include <math.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "libenergymsr.h"

typedef struct
{
   unsigned int nbCPUs; // number of CPUs on the system
   int *fds;            // MSR files
   float *eUnits;       // Coefficients to convert energy in J
   double *lastE;       // last measured energy per socket
   double *refE;        // first measured energy per socket
   unsigned int *ovfCnt;   // overflow counter per socket

   double *res;
} libdata_t;

#define MSR_RAPL_POWER_UNIT   0x606
#define MSR_PKG_ENERGY_STATUS 0x611

/**
 * Returns a subvalue made of some contiguous bits in the value.
 *
 * @param val The value in which reading the subvalue.
 * @param pos The subvalue position.
 * @param length The subvalue length.
 *
 * @return The subvalue of val at the given position and specified length.
 */
static uint64_t subvalue(uint64_t val, unsigned int pos, unsigned int length)
{
   return (val >> pos) & ~(0xFFFFFFFFFFFFFFFFL << length);
}

extern unsigned int nbDevices() {
   const unsigned int nb_cores = sysconf(_SC_NPROCESSORS_ONLN);
   unsigned int i, nb_sockets = 1;

   for (i = 0; i < nb_cores; i++) {
      char path[64];
      unsigned int sid;

      snprintf(path, 64, "/sys/devices/system/cpu/cpu%u/topology/physical_package_id", i);
      FILE *fd = fopen(path, "r");
      if (fd == NULL) {
         continue;
      }

      if (fscanf(fd, "%u", &sid) < 1) {
         fclose(fd);
         continue;
      }
      fclose(fd);

      if (sid + 1 > nb_sockets) {
         nb_sockets = sid + 1;
      }
   }

   return nb_sockets;
}

extern unsigned int nbChannels() {
   return 1;
}

extern void *init (void) {
   libdata_t *data = malloc(sizeof(*data));

   data->nbCPUs = nbDevices();
   data->fds = malloc(data->nbCPUs * sizeof(*data->fds));
   for (i = 0; i < ldata->nbCPUs; i++) {
      data->fds[i] = -1;
   }

   data->eUnits = malloc(data->nbCPUs * sizeof(*data->eUnits));
   data->lastE = malloc(data->nbCPUs * sizeof(*data->lastE));
   data->refE = malloc(data->nbCPUs * sizeof(*data->refE));
   data->ovfCnt = malloc(data->nbCPUs * sizeof(*data->ovfCnt));
   data->res = malloc(data->nbCPUs * sizeof(*data->res));

   // open one MSR file per socket
   unsigned int s, c;
   const unsigned int nb_cores = sysconf(_SC_NPROCESSORS_ONLN);
   for (s = 0; s < ldata->nbCPUs; s++)
   {
      char path[64];

      // find a core on socket s
      for (c = 0; c < nb_cores; c++)
      {
         FILE *fd;
         unsigned int sock_id;

         snprintf(path, 64, "/sys/devices/system/cpu/cpu%u/topology/physical_package_id", c);
         if ((fd = fopen(path, "r")) == NULL) {
            continue;
         }

         if (fscanf(fd, "%u", &sock_id) < 1) {
            fclose(fd);
            continue;
         }
         fclose(fd);

         if (sock_id == s) {
            break;
         }
      }

      if (c >= nb_cores) {
         printf("Failed to find a valid core on socket %u\n", s);
         return;
      }

      snprintf(path, 64, "/dev/cpu/%u/msr", c);

      if ((data->fds[s] = open(path, O_RDONLY)) < 0) {
         perror("Failed to open MSR file");
         return;
      }

      // read the MSR to fetch energy unit
      uint64_t energy_unit;
      pread(data->fds[s], &energy_unit, sizeof(energy_unit), MSR_RAPL_POWER_UNIT);
      data->eUnits[s] = pow(0.5, subvalue(energy_unit, 8, 5));
   }
}

extern void fini (void *data) {
   unsigned int i;
   libdata_t *ldata = (libdata_t) data;

   for (i = 0; i < ldata->nbCPUs; i++) {
      if (ld->fds[i] != -1) {
         close(ldata->fds[i]);
      }
   }

   free(ldata->ovfCnt);
   free(ldata->lastE);
   free(ldata->refE);
   free(ldata->eUnits);
   free(ldata->fds);
   free(ldata->res);
   free(ldata);
}

extern void start (void *data) {
   unsigned int s;
   uint64_t e_base;
   libdata_t *ldata = (libdata_t) data;

   // init the structures with the current counter values
   for (s = 0; s < ldata->nbCPUs; s++)
   {
      if (pread(ldata->fds[s], &e_base, sizeof(e_base), MSR_PKG_ENERGY_STATUS) < (ssize_t) sizeof(e_base))
      {
         perror("Failed to read MSR register\n");
         continue;
      }
      e_base = subvalue(e_base, 0, 32);

      ldata->refE[s] = e_base * ldata->eUnits[s];
      ldata->lastE[s] = ldata->refE[s];
      ldata->ovfCnt[s] = 0;
      ldata->res[s] = 0;
   }
}

extern double *stop (void *data) {
   unsigned int s;
   libdata_t *ldata = (libdata_t) data;

   // get fresh data when we stop
   update(data);

   for (s = 0; s < ldata->nbCPUs; s++) {
      ldata->res[s] = ldata->lastE[s] + (ldata->ovfCnt[s] * (UINT32_MAX + 1.) * ldata->eUnits[s]) - ldata->refE[s];
   }

   return ldata->res;
}

extern void update (void *data) {
   unsigned int s;
   uint64_t e_base;
   libdata_t *ldata = (libdata_t) data;

   for (s = 0; s < ldata->nbCPUs; s++)
   {
      // current counter value
      if (pread(ldata->fds[s], &e_base, sizeof(e_base), MSR_PKG_ENERGY_STATUS) < (ssize_t) sizeof(e_base))
      {
         perror("Failed to read MSR register\n");
         continue;
      }
      e_base = subvalue(e_base, 0, 32);
      double energy = e_base * ldata->eUnits[s];

      // check for overflows
      if (energy < ldata->lastE[s]) {
         ldata->ovfCnt[s]++;
      }

      ldata->lastE[s] = energy;
   }
}
