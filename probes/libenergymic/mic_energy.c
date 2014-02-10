/*
 * lPowerProbe - A light benchmark tool oriented for energy probing
 *               heavely based on likwid
 * Copyright (C) 2013-2014 Universite de Versailles
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#define POWER_POLLING_INTERVAL 100 // (us)

typedef struct sEnergyInfo {
   pthread_t thr;
   struct timespec start, stop;
   double averagePower;
#ifdef TRACING
   FILE *traceFd;
#endif
} EnergyInfo;

void *gatherPowerInfo (void *info);

static inline unsigned long readPower () {
   FILE* fp;
   fp = fopen("/sys/class/micras/power", "r");
   if (!fp) {
      perror ("Cannot find power file. Are you running this library on mic ?");
      exit (EXIT_FAILURE);
   } 

   unsigned long powerPCIe = 0, dummy = 0;
   
   fscanf (fp, "%lu", &dummy);
   fscanf (fp, "%lu", &dummy);
   fscanf (fp, "%lu", &powerPCIe); 

   fclose (fp), fp = NULL;

   return powerPCIe;
}

void *evaluationInit () {
   EnergyInfo *info = malloc (sizeof (*info));
   assert (info != NULL);
   info->averagePower = 0;
   return info;
}

static inline void getTime (struct timespec *tp) {
   if (clock_gettime (CLOCK_MONOTONIC, tp) == -1) {
      perror ("FATAL: clock_gettime error");
      exit (EXIT_FAILURE);
   } 
}

void *gatherPowerInfo (void *nfo) {
   EnergyInfo *info = (EnergyInfo*) nfo;
   assert (info != NULL);
   info->averagePower = readPower ();
   uint64_t nbIterations = 0;

#ifdef TRACING
   info->traceFd = fopen ("trace.txt", "a");
   struct timespec currentTime;
   if (!info->traceFd) {
      perror ("Cannot open tracing file in append mode");
      exit (EXIT_FAILURE);
   }
#endif

   while (1) { 
     // Measure exactly how long we sleep in timeSpent
     usleep (POWER_POLLING_INTERVAL);
     
     // Get current power
     unsigned long currentPower = readPower ();

#ifdef TRACING
     getTime (&currentTime); 
     uint64_t ntime = currentTime.tv_sec * 1e9 + currentTime.tv_nsec;
     fprintf (info->traceFd, "%llu %lu\n", ntime, currentPower);
#endif
 
     // Add up the energy consumed 
     info->averagePower = (currentPower + info->averagePower*nbIterations) / (nbIterations + 1);
     nbIterations++;
   }
   
   return NULL;
}

double evaluationStart (void *nfo) {
   EnergyInfo *info = (EnergyInfo*) nfo;
   assert (info != NULL);
   //fprintf (stderr, "start\n");
   getTime (&info->start);
   if (pthread_create (&info->thr, NULL, gatherPowerInfo, info) != 0) {
      fprintf (stderr, "Pthread create error\n");
      exit (EXIT_FAILURE);
   }
   (void) nfo;
   return 0;
}

double evaluationStop (void *nfo) {
   EnergyInfo *info = (EnergyInfo*) nfo;
   assert (info != NULL);
   //fprintf (stderr, "stop\n");
   pthread_cancel (info->thr);
   pthread_join (info->thr, NULL);

#ifdef TRACING
   if (info->traceFd != NULL) {
      struct timespec currentTime;
      getTime (&currentTime);
      uint64_t ntime = currentTime.tv_sec * 1e9 + currentTime.tv_nsec;   
      fprintf (info->traceFd, "%llu 0\n", ntime);
      fclose (info->traceFd), info->traceFd = NULL;
   }
#endif
   getTime (&info->stop);
   
   uint64_t timeSpent = (info->stop.tv_sec*1000000000 + info->stop.tv_nsec)
       - (info->start.tv_sec*1000000000 + info->start.tv_nsec);
   double energy = (info->averagePower / 1000000) * ((double)timeSpent / 1000000000);
   //fprintf (stderr, "ENDSTOP, returning energy = %f, ts = %lu, info->averagePower = %f\n", energy, timeSpent, info->averagePower);

   return energy;
}

int evaluationClose (void *nfo) {
   free (nfo), nfo = NULL;
   return EXIT_SUCCESS;
}

