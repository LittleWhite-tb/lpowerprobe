/*
 * ===========================================================================
 *
 *      Filename:  types.h
 *
 *      Description:  Global  Types file
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


#ifndef TYPES_H
#define TYPES_H

/* #####   HEADER FILE INCLUDES   ######################################### */
#include <stdint.h>

#include "msr_types.h"

/* #####   EXPORTED MACROS   ############################################## */

#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif
#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif

#define TRUE  1
#define FALSE 0

#define HLINE "-------------------------------------------------------------\n"
#define SLINE "*************************************************************\n"

#define LLU_CAST  (unsigned long long)

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#endif /*TYPES_H*/
