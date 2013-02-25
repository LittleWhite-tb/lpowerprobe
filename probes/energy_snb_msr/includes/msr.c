/*
 * ===========================================================================
 *
 *      Filename:  msr.c
 *
 *      Description:  Implementation of msr module.
 *                   Provides API to read and write values to the model
 *                   specific registers on x86 processors using the msr
 *                   sys interface of the Linux 2.6 kernel. This module 
 *                   is based on the msr-util tools.
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


/* #####   HEADER FILE INCLUDES   ######################################### */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h> 

#include "types.h"
#include "error.h"
#include "msr.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

static int socket_fd = -1;
static char* filepath;

static char * msr_strerror(MsrErrorType met);

static char * msr_strerror(MsrErrorType met)
{
    switch (met) {
        case MSR_ERR_NOERROR:   return "No error";
        case MSR_ERR_UNKNOWN:   return "unknown command";
        case MSR_ERR_RESTREG:   return "access to this MSR is not allowed";
        case MSR_ERR_OPENFAIL:  return "failed to open MSR file";
        case MSR_ERR_RWFAIL:    return "failed to read/write MSR";
        default:                return "UNKNOWN errorcode";
    }
}

/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ################## */

void
msr_init()
{
    int  fd;
    char* msr_file_name = "/dev/cpu/0/msr";

    fd = open(msr_file_name, O_RDWR);

    if (fd < 0)
    {
        fprintf(stderr, "ERROR\n");
        fprintf(stderr, "rdmsr: failed to open '%s': %s!\n",msr_file_name , strerror(errno));
        fprintf(stderr, "       Please check if the msr module is loaded and the device file has correct permissions.\n\n");
        exit(127);
    }

    close(fd);
}

void
msr_finalize()
{
    if (socket_fd != -1) { /* Only if a socket is actually open */
        MsrDataRecord msrData;
        msrData.type = EXIT;
        CHECK_ERROR(write(socket_fd, &msrData, sizeof(MsrDataRecord)),socket write failed);
        CHECK_ERROR(close(socket_fd),socket close failed);
        free(filepath);
    }
}


uint64_t 
msr_read(const int cpu, uint32_t reg)
{
    int  fd;
    uint64_t data;
    char msr_file_name[64];

    sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);
    fd = open(msr_file_name, O_RDONLY);
    if (fd < 0)
    {
        ERROR;
    }

    if (pread(fd, &data, sizeof data, reg) != sizeof data) 
    {
        ERROR;
    }

    close(fd);

    return data;
}

void 
msr_write(const int cpu, uint32_t reg, uint64_t data)
{
    int  fd;
    char msr_file_name[64];

    sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);
    fd = open(msr_file_name, O_WRONLY);

    if (fd < 0) 
    {
        ERROR;
    }

    if (pwrite(fd, &data, sizeof data, reg) != sizeof data) 
    {
        ERROR;
    }

    close(fd);
}


