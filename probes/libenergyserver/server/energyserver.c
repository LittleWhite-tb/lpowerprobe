#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "energyserver.h"

// redefine here the functions and variables taken from libenergymsr
// do not include the header as everything is declared as extern
const unsigned int period;
unsigned int nbDevices();
void *init (void);
void fini (void *data);
void start (void *data);
double *stop (void *data);
void update (void *data);

static void *energy_ctx;
static server_data *shared;

// signal handler for SIGTERM and SIGINT
static void handler_fn(int snum) {
   (void) snum;
   
   stop(energy_ctx);
   fini(energy_ctx);
   shm_unlink(ESRV_SHM_NAME);
   munmap(shared, sizeof(server_data));
}

// entry point
int main(int argc, char **argv) {
   unsigned int i;

   (void) argc;
   (void) argv;

   // check the number of devices
   if (nbDevices() > ESRV_MAX_NB_DEVICES) {
      fprintf(stderr, "Too many MSR counters to read!\n");
      return EXIT_FAILURE;
   }

   // ensure we have correct permissions
   umask(0000);   // allow everything!

   // create the shared memory segment
   int fd = shm_open(ESRV_SHM_NAME, O_RDWR | O_CREAT | O_TRUNC, 0666);
   if (fd < 0) {
      perror("Failed to create shared memory " ESRV_SHM_NAME);
      return EXIT_FAILURE;
   }

   // resize the shared data segment
   if (ftruncate(fd, sizeof(server_data)) < 0) {
      perror("Failed to resize shared memory");
      shm_unlink(ESRV_SHM_NAME);
      return EXIT_FAILURE;
   }

   // map it onto memory
   shared = mmap(NULL, sizeof(server_data), PROT_READ | PROT_WRITE,
                            MAP_SHARED, fd, 0);
   if (shared == NULL) {
      perror("Failed to map shared memory");
      shm_unlink(ESRV_SHM_NAME);
      return EXIT_FAILURE;
   }
   close(fd);

   // start energy profiling (from libenergymsr)
   energy_ctx = init();
   if (energy_ctx == NULL) {
      fprintf(stderr, "Failed to fetch energy information\n");
      shm_unlink(ESRV_SHM_NAME);
      munmap(shared, sizeof(server_data));
      return EXIT_FAILURE;
   }
   start(energy_ctx);

   // capture killing signals
   struct sigaction sig_handler;

   sigemptyset(&sig_handler.sa_mask);
   sig_handler.sa_handler = handler_fn;

   sigaction(SIGTERM, &sig_handler, NULL);
   sigaction(SIGINT, &sig_handler, NULL);

   // initialize shared data
   sem_init(&shared->measure, 1, 0);
   sem_init(&shared->read, 1, 0);
   shared->nb_devices = nbDevices();
   for (i = 0; i < shared->nb_devices; ++i) {
      shared->energy[i] = 0;
   }

   // while not interrupted
   while (1) {
      double *last_results;   
      struct timespec ts;

      clock_gettime(CLOCK_REALTIME_COARSE, &ts);
      ts.tv_sec += period / 1000000;   // seconds are enough

      if (sem_timedwait(&shared->measure, &ts) < 0) {
         if (errno == ETIMEDOUT) {
            update(energy_ctx);
            continue;
         } else {
            perror("Error while waiting requests");
         }
      }

      // read energy
      last_results = stop(energy_ctx);
      for (i = 0; i < shared->nb_devices; ++i) {
         shared->energy[i] = last_results[i];
      }

      // get ready for the next request
      sem_post(&shared->read);
      start(energy_ctx);
   }

   // never reached
   return EXIT_SUCCESS;
}
