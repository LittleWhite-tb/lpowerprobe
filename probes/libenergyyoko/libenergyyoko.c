/*
 * Copyright (C) 2013-2014 Universite de Versailles 
 * Copyright (C) 2012 Exascale Research Center
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <stdlib.h>

#include "libenergyyoko.h"
#include "yokosrv.h"


typedef struct {
  yokoctx_t *ctx;
  double e;
} libdata_t;

extern unsigned int nbDevices(void *data) {
   (void) data;
   return 1;
}

extern unsigned int nbChannels(void *data) {
   (void) data;
   return 1;
}

extern void *init (void) {
  libdata_t *res = malloc(sizeof(*res));

  res->ctx = yoko_connect();
  res->e = 0;

  return res;
}

extern void fini (void *data) {
  yoko_disconnect(((libdata_t *) data)->ctx);

  free(data);
}

extern void start (void *data) {
  yoko_read_energy(data);
}

extern double *stop (void *data) {
  libdata_t *ldata = data;

  ldata->e = yoko_read_energy(data);
  return &ldata->e;
}

extern void update (void *data) {
  (void) data;
}

