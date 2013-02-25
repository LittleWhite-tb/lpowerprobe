/*
 * ===========================================================================
 *
 *      Filename:  msr_types.h
 *
 *      Description:  Types file for msr module.
 *
 *      Version:  <VERSION>
 *      Created:  <DATE>
 *
 *      Author:  Jan Treibig (jt), jan.treibig@gmail.com
 *      Company:  RRZE Erlangen
 *      Project:  likwid
 *      Copyright:  Copyright (c) 2010, Jan Treibig
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License, v2, as
 *      published by the Free Software Foundation
 *     
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *     
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * ===========================================================================
 *
 * lPowerProbe - A light benchmark tool oriented for energy probing
 *               heavely based on likwid
 * Copyright (C) 2013 Universite de Versailles
 * 2011-08: Modified by Exascale Research Center
 */

#include <stdint.h>

#ifndef MSR_TYPES_H
#define MSR_TYPES_H

typedef enum {
    MSR_READ = 0,
    MSR_WRITE,
    EXIT
} MsrAccessType;

typedef enum {
    MSR_ERR_NOERROR = 0,  /* no error */
    MSR_ERR_UNKNOWN,      /* unknown command */
    MSR_ERR_RESTREG,      /* attempt to access restricted MSR */
    MSR_ERR_OPENFAIL,     /* failure to open msr files */
    MSR_ERR_RWFAIL        /* failure to read/write msr */
} MsrErrorType;

typedef struct {
    uint32_t cpu;
    uint32_t reg;
    uint64_t data;
    MsrAccessType type;
    MsrErrorType errorcode; /* Only in replies - 0 if no error. */
} MsrDataRecord;

#endif /*MSR_TYPES_H*/
