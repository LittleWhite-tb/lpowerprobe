/*
 * Copyright (C) 2013-2014 Universite de Versailles
 * Copyright (C) 2012 Exascale Research Center

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <assert.h>

#include <perfmon/pfmlib.h>
#include <perfmon/perf_event.h>
#include <perfmon/pfmlib_perf_event.h>
#include "inih/ini.h"

#ifndef LPP_API_VERSION
#error LPP_API_VERSION undefined, never call this makefile directly!
#endif

const unsigned int version = LPP_API_VERSION;
const char *label = "Time (us)";
const unsigned int period = 0;

typedef struct {
   short int is_open;
   int *pfmFds;
   int *cores;
   size_t nbCores;
   char **counterNames;
   size_t nbCounters;
   double *values;
} SContext;

extern unsigned int nbDevices (void *data) {
   (void) data;
   return 1;
}

extern void update (void *data) {
   (void) data;
}
/**
 * @brief Callback for parsing the INI file.
 *
 * @param user The current context.
 * @param section The current section we're in.
 * @param name The property name.
 * @param value The property value.
 *
 * @return 0 on failure, non zero otherwise.
 */
static int iniHandler(void* user, const char* section, const char* name,
                       const char* value)
{
   SContext *ctx = (SContext *) user;
   unsigned int i;

   (void) (section);

   if (!strcmp(name, "counters"))
   {
      char *buf = strdup(value);
      char *tokintern;
      char *tmp, *tok;

      // count the counters
      ctx->nbCounters = 1;
      for (tmp = buf; *tmp != '\0'; tmp++)
      {
         if (*tmp == ';')
         {
            ctx->nbCounters++;
         }
      }
      //fprintf (stderr, "allocating %u for counternames\n", (unsigned int)ctx->nbCounters);
      ctx->counterNames = malloc(ctx->nbCounters * sizeof(*ctx->counterNames));
      
      // get all the counter names
      for (i = 0, tmp = buf;
           (tok = strtok_r(tmp, ";", &tokintern)) != NULL;
           i++, tmp = NULL)
      {
         ctx->counterNames[i] = strdup(tok);
         //fprintf (stderr, "names [i] = %s\n", ctx->counterNames [i]);
      }

      free(buf);
   }
   else if (!strcmp(name, "nbCores"))
   {
      unsigned int i;
      ctx->nbCores = strtoul(value, NULL, 10);
      ctx->cores = malloc (ctx->nbCores * sizeof (*ctx->cores));
      //fprintf (stderr, "Allocating %u cores\n", (unsigned int)ctx->nbCores);
      
      for (i = 0; i < ctx->nbCores; i++) {
         ctx->cores [i] = i;
      }
   }

   return 1;
}

extern unsigned int nbChannels (void *data) {
   SContext *ctx = (SContext*) data;
   assert (ctx != NULL);
   return ctx->nbCores * ctx->nbCounters;
}

static inline SContext *Context_init () {
   SContext *ctx = malloc (sizeof (*ctx));
   assert (ctx != NULL);
   memset (ctx, 0, sizeof (*ctx));

   // Parse ini file 
   int err = ini_parse ("probes/pfmcounters/pfmcounters.ini", iniHandler, ctx);
   if (err == -1) {
      fprintf (stderr, "An error occured while parsing ini file, error = %d\n", err);
      exit (EXIT_FAILURE);
   }
   
   // Allocate the file descriptors
   unsigned int tabSize = ctx->nbCores * ctx->nbCounters;
   unsigned int fdsSize = tabSize * sizeof (*ctx->pfmFds);
   unsigned int valuesSize = tabSize * sizeof (*ctx->values);
   ctx->pfmFds = malloc (fdsSize);
   assert (ctx->pfmFds != 0);
   memset (ctx->pfmFds, 0, fdsSize);
   ctx->values = malloc (valuesSize);
   assert (ctx->values != 0);
   memset (ctx->values, 0, valuesSize);
   ctx->is_open = 0;

   return ctx;
}

static inline void Context_destroy (SContext *ctx) {
   unsigned int i;

   if (ctx == NULL ) {
      return;
   }

   // close all counters if opened
   if (ctx->is_open) {
      for (i = 0; i < ctx->nbCounters*ctx->nbCores; i++)
      {
         close (ctx->pfmFds[i]);
      }
   }

   // Free memory
   if (ctx->pfmFds != NULL) {
      free (ctx->pfmFds), ctx->pfmFds = NULL;
   }

   if (ctx->cores != NULL) {
      free (ctx->cores), ctx->cores = NULL;
   }

   if (ctx->counterNames != NULL) {
      for (i = 0; i < ctx->nbCounters; i++) {
         if (ctx->counterNames [i] != NULL) {
            free (ctx->counterNames [i]), ctx->counterNames [i] = NULL;
         }
      }
      free (ctx->counterNames), ctx->counterNames = NULL;
   }
   free (ctx), ctx = NULL;
}

/**
 * @brief Called to initialize the library.
 *
 * @return a handler for this specific instance of the library.
 */
extern void *init (void)
{
   unsigned int coreId, cntId;
   int tmp;
   SContext *ctx = Context_init ();
   
   // initialize the libpfm library
   if ((tmp = pfm_initialize ()) != PFM_SUCCESS)
   {
      fprintf (stderr, "Failed to initialize libpfm: %s\n", pfm_strerror(tmp));
	   free (ctx);
	   return NULL;
   }

   // initialize counters
   for (coreId = 0; coreId < ctx->nbCores; coreId++) {
      for (cntId = 0; cntId < ctx->nbCounters; cntId++)
      {
         pfm_perf_encode_arg_t arg;
         struct perf_event_attr attr;
   
         // initialize the structure
         memset(&attr, 0, sizeof(attr));
         memset(&arg, 0, sizeof(arg));
         arg.attr = &attr;
         arg.fstr = NULL;
         arg.size = sizeof(pfm_perf_encode_arg_t);
   
         // encode the counter
         tmp = pfm_get_os_event_encoding (ctx->counterNames [cntId], PFM_PLM0 | PFM_PLM3,
                                          PFM_OS_PERF_EVENT_EXT, &arg);
         if (tmp != PFM_SUCCESS)
         {
            fprintf(stderr, "Failed to get counter %s on core %d\n",
                    ctx->counterNames[cntId],
                    ctx->cores [coreId]);
   	      free(ctx);
   	      return NULL;
         }
   
         // request scaling in case of shared counters
         arg.attr->read_format = PERF_FORMAT_TOTAL_TIME_ENABLED
                                 | PERF_FORMAT_TOTAL_TIME_RUNNING;
         
         // Xeon Phi hack
         arg.attr->exclude_guest = 0;

         // open the file descriptor
         unsigned int idx = coreId * ctx->nbCounters + cntId;
         ctx->pfmFds[idx] = perf_event_open (&attr, -1, ctx->cores [coreId], -1, 0);
         if (ctx->pfmFds[idx] == -1)
         {
            err(1, "Cannot open counter %s on core %d",
                  ctx->counterNames[cntId],
                  ctx->cores [coreId]);
            free(ctx);
   	      return NULL;
         }
   
         // Activate the counter
         if (ioctl (ctx->pfmFds[idx], PERF_EVENT_IOC_ENABLE, 0))
         {
            fprintf(stderr, "Cannot enable event %s\n", ctx->counterNames[cntId]);
   	      free(ctx);
   	      return NULL;
         }
      }
   }
   ctx->is_open = 1;
   return ctx;
}

/**
 * @brief Freeing memory, closing stuff, ...
 */
extern void fini (void *data)
{ 
   SContext *ctx = (SContext*) data;
   
   // Delete the context
   Context_destroy (ctx);

   // Close pfm
   pfm_terminate ();
}

extern void start (void *data)
{
   SContext *ctx = (SContext*) data;

   unsigned int coreId, cntId;
   int tmp;
   
   if (ctx == NULL || ctx->nbCounters == 0)
   {
      return;
   }

   // read initial counter value
   for (coreId = 0; coreId < ctx->nbCores; coreId++) {
      for (cntId = 0; cntId < ctx->nbCounters; cntId++)
      {
         uint64_t buf [3];
         unsigned idx = coreId * ctx->nbCounters + cntId;
      
         tmp = read (ctx->pfmFds[idx], buf, sizeof (buf));
         if (tmp != sizeof (buf))
         {
            fprintf(stderr, "Failed to read counter %s for core %d\n", ctx->counterNames[cntId], ctx->cores [coreId]);
         }

         // handle scaling to allow PMU sharing
         ctx->values [idx] = (uint64_t)((double) buf [0] * buf [1] / buf [2]);
      }
   }
}

extern double *stop (void *data)
{
   SContext *ctx = (SContext*) data;

   unsigned int coreId, cntId;
   int tmp; 

   // just in case...
   assert (ctx != NULL);
   assert (ctx->nbCounters != 0); 

   // read values
   for (coreId = 0; coreId < ctx->nbCores; coreId++) {
      for (cntId = 0; cntId < ctx->nbCounters; cntId++)
      {
         uint64_t buf [3];
         uint64_t value;
         unsigned int idx = coreId * ctx->nbCounters + cntId;
   
         tmp = read (ctx->pfmFds[idx], buf, sizeof (buf));
         if (tmp != sizeof (buf))
         {
            fprintf(stderr, "Failed to read counter %s\n", ctx->counterNames[cntId]);
         }
   
         // handle scaling to allow PMU sharing
         value = (uint64_t)((double) buf [0] * buf [1] / buf [2]);
         if (ctx->values [idx] <= value)
         {
            ctx->values [idx] = value - ctx->values [idx];
         }
         else  // overflow
         {
            ctx->values [idx] = 0xFFFFFFFFFFFFFFFFUL - ctx->values [idx] + value;
         }
      }
   }
   
   return ctx->values;
}

