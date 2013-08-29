/*
 * lPowerProbe - A light benchmark tool oriented for energy probing
 *               heavely based on likwid
 * Copyright (C) 2013 Universite de Versailles
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

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include "mpi.h"

static pid_t lpp_pid;

int MPI_Init(int *argc, char ***argv)
{
   void *real_fun;
   (void) argc;
   (void) argv;
   int real_res;
   FILE *fd;

   // only the process with local rank 0 signals itself
   char *lrank = getenv("OMPI_COMM_WORLD_LOCAL_RANK");
   if (atol(lrank) == 0) {
      // where is lpp?
      fd = fopen("/tmp/lppDaemonPID", "r");
      if (fscanf(fd, "%u", &lpp_pid) < 1) {
         fprintf(stderr, "Failed to read the lPowerProbe pid\n");
         exit(EXIT_FAILURE);
      }
      fclose(fd);
   } else {
      lpp_pid = 0;
   }

   // call the real init
   real_fun = dlsym(RTLD_NEXT, "MPI_Init");
   real_res = ((int (*)(int *argc, char ***argv)) real_fun)(argc, argv);

   // signal lpp we started
   if (lpp_pid) {
      kill(lpp_pid, SIGUSR1);
   }

   return real_res;
}

int MPI_Finalize()
{
   // wait for everyone
   MPI_Barrier(MPI_COMM_WORLD);

   // signal lpp we ended
   if (lpp_pid) {
      kill(lpp_pid, SIGUSR1);
   }

   // call the real finalize
   void *real_fun = dlsym(RTLD_NEXT, "MPI_Finalize");
   return ((int (*)()) real_fun)();
}
