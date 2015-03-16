/*
Copyright (C) 2015 Exascale Research Center

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

#include "barrier.h"

#include <assert.h>

#include <stdio.h>
#include <errno.h>

int barrier_create(Barrier* b, unsigned int count)
{
   int error = 0;
   
   assert(b);
   assert(count != 0);
   
   b->count = 0;
   b->nbToWait = count;
   error = pthread_mutex_init(&b->mutex, NULL);
   if ( error != 0 )
   {
      fprintf(stderr,"[TIMERANDROID] Fail to create mutex for barrier '%s'\n",strerror(errno));
      return -1;
   }
   
   error = pthread_cond_init(&b->cond, NULL);
   if ( error != 0 )
   {
      fprintf(stderr,"[TIMERANDROID] Fail to create condition for barrier '%s'\n",strerror(errno));
      return -2;
   }
   
   return 0;
}

int barrier_wait(Barrier* b)
{
   assert(b);
   
   pthread_mutex_lock(&b->mutex);
      b->count++;
      if (b->count == b->nbToWait) 
      { 
         fprintf(stderr,"[TIMERANDROID] Barrier broadcast (value : %d)\n",b->count);
         pthread_cond_broadcast(&b->cond);
         b->count = 0;
      }
      else 
      {
         fprintf(stderr,"[TIMERANDROID] Barrier cond wait (value : %d)\n",b->count);
         pthread_cond_wait(&b->cond, &b->mutex);
      }
   pthread_mutex_unlock(&b->mutex);
   
   return 0;
}

int barrier_destroy(Barrier* b)
{
   fprintf(stderr,"[TIMERANDROID] Barrier destruction\n");
   pthread_cond_broadcast(&b->cond); 
   
   pthread_mutex_destroy(&b->mutex);
   pthread_cond_destroy(&b->cond);
   
   return 0;
}
