/*
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
/*============================================================================
   PROJECT : Kernel_Mem_Benchmark (kerbe)
   SOURCE  : timer.c
   MODULE  : Performance Counters
   AUTHOR  : Christophe Lemuet
   UPDATED : 09/09/2003 (DD/MM/YYYY)
   UPDATED : 20/09/2005 (DD/MM/YYYY) JT.Acquaviva. Add X86 timer with rdtsc() function
============================================================================*/

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "timer.h"

// Define rdtscll for x86_64 arch
#ifdef __x86_64__
   #define rdtscll(val) do { \
         unsigned int __a,__d; \
         asm volatile("rdtsc" : "=a" (__a), "=d" (__d)); \
         (val) = ((unsigned long)__a) | (((unsigned long)__d)<<32); \
   } while(0)
#endif

// Define rdtscll for i386 arch
#ifdef __i386__
   #define rdtscll(val) { \
      asm volatile ("rdtsc" : "=A"(val)); \
      }
#endif

const unsigned int version = LPP_API_VERSION;
const char *label = "Cycles";
const unsigned int period = 0;

typedef struct sData {
   double tick;
} SData;
       
static __inline__ unsigned long long getticks(void)
{
   unsigned long long ret;
   rdtscll(ret);
   return ret;
}

extern unsigned int nbDevices (void *data) {
   (void) data;
   return 1;
}

extern unsigned int nbChannels (void *data) {
   (void) data;
   return 1;
}

extern void *init (void)
{
   SData *data = malloc (sizeof (*data));
   assert (data != NULL);
	return data;
}

extern void start (void *data)
{ 
	SData *dat = (SData*) data;
   dat->tick = getticks ();
}

extern double *stop (void *data)
{
   SData *dat = (SData*) data;
   dat->tick = getticks () - dat->tick;
	return &(dat->tick);
}

extern void fini (void *data) {
   SData *dat = (SData*) data;
   free (dat), dat = NULL;
}

extern void update (void *data) {
   (void) data;
}
