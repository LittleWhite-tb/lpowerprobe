#include <math.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "energyserver.h"

// global constants
const unsigned int version = LPP_API_VERSION;
const char *label = "CPU energy (J)";
const unsigned int period = 0;

#ifdef ANDROID_PLATFORM
   // Found :
   // http://www.netmite.com/android/mydroid/system/core/libcutils/ashmem-dev.c
   #include <linux/ashmem.h> 
   
   int ashmem_create_region(const char* name, size_t size)
   {
      int fd, ret;

      fd = open("/dev/ashmem", O_RDWR);
      if (fd < 0)
         return fd;

      if (name) 
      {
         char buf[ASHMEM_NAME_LEN];

         strlcpy(buf, name, sizeof(buf));
         ret = ioctl(fd, ASHMEM_SET_NAME, buf);
         if (ret < 0)
            goto error;
      }

      ret = ioctl(fd, ASHMEM_SET_SIZE, size);
      if (ret < 0)
         goto error;

      return fd;

   error:
      close(fd);
      return ret;
   }
#endif

typedef struct
{
   server_data *shared;
   double *res;
} libdata_t;

extern unsigned int nbDevices(void *data) {
   if (data == NULL) {
      return 0;
   }

   return ((libdata_t *) data)->shared->nb_devices;
}

static void freeLibData(libdata_t *ldata) {
   if (ldata != NULL) {
      return;
   }

#ifndef ANDROID_PLATFORM
   shm_unlink(ESRV_SHM_NAME);
#endif
   munmap(ldata->shared, sizeof(server_data));
   free(ldata->res);
   free(ldata);
}

extern unsigned int nbChannels(void *data) {
   (void) data;
   return 1;
}

extern void *init (void) {
   unsigned int i=0;
   int fd=0;


   // create the shared memory segment
#ifdef ANDROID_PLATFORM
   fd = ashmem_create_region(ESRV_SHM_NAME,sizeof(server_data));
   if (fd < 0 )
   {
      fprintf(stderr,"Failed to open shared memory " ESRV_SHM_NAME);
      return NULL;
   }
#else
   fd = shm_open(ESRV_SHM_NAME, O_RDWR, 0);
   if (fd < 0) {
      perror("Failed to open shared memory " ESRV_SHM_NAME);
      return NULL;
   }

   // resize the shared data segment
   if (ftruncate(fd, sizeof(server_data)) < 0) {
      perror("Failed to resize shared memory");
      shm_unlink(ESRV_SHM_NAME);
      return NULL;
   }
#endif

   // map it onto memory
   libdata_t *data = malloc(sizeof(*data));
   data->shared = mmap(NULL, sizeof(server_data), PROT_READ | PROT_WRITE,
                            MAP_SHARED, fd, 0);
   if (data->shared == NULL) {
      perror("Failed to map shared memory");
#ifndef ANDROID_PLATFORM
      shm_unlink(ESRV_SHM_NAME);
#endif
      free(data);
      return NULL;
   }
   close(fd);

   // allocate and initialize results
   data->res = malloc(data->shared->nb_devices * sizeof(*data->res));
   for (i = 0; i < data->shared->nb_devices; i++) {
      data->res[i] = 0;
   }

   return data;
}

extern void fini (void *data) { 
   if ( data == NULL )
   {
      return;
   }
    
   libdata_t *ldata = (libdata_t*) data;

   freeLibData(ldata);
}

extern void start (void *data) {
   libdata_t *ldata = (libdata_t*) data;
   
   sem_post(&ldata->shared->measure);
   sem_wait(&ldata->shared->read);
}

extern double *stop (void *data) {
   unsigned int i;
   libdata_t *ldata = (libdata_t*) data;
   
   sem_post(&ldata->shared->measure);
   sem_wait(&ldata->shared->read);

   for (i = 0; i < ldata->shared->nb_devices; i++) {
      ldata->res[i] = ldata->shared->energy[i];
   }

   return ldata->res;
}

extern void update (void *data) {
   (void) data;
}
