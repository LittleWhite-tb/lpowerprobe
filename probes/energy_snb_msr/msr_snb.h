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
   SOURCE  : msr_snb.h
   MODULE  : Power Counters
   AUTHOR  : Hugo Bolloré
   UPDATED : 03/08/2011 (DD/MM/YYYY)
============================================================================*/

#ifndef H_MSR_SNB
#define H_MSR_SNB

#define POWER_POLLING_INTERVAL 20 //must be below 60 seconds since this is the worst case wrap around time

void *evaluationInit (void);
int evaluationClose (void *);
double evaluationStart (void *);
double evaluationStop (void *);

#endif
