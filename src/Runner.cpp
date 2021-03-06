/*
 * lPowerProbe - A light benchmark tool oriented for energy probing
 *               heavely based on likwid
 * Copyright (C) 2013-2014 Universite de Versailles
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Runner.hpp"

#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h> 


#include "StringUtils.hpp" 

#include "CPUUtils.hpp" 

#include "RunData.hpp"
#include "ProbeData.hpp"

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

Runner::Runner(ProbeDataCollector* pProbesDataCollector, unsigned int nbProcess, unsigned int nbMetaRepet)
   :m_pProbesDataCollector(pProbesDataCollector),m_nbMetaRepet(nbMetaRepet),m_nbProcess(nbProcess)
{

   // Get pid to put in semaphores names
   m_pid = getpid();
   // Convert it
   if ( StringUtils::to_string(m_pid,m_pidString) == false )
   {
      std::cerr << "Fail to transform PID in a string" << std::endl;
      m_pidString = "00000";
   }
   
   
   // open fatherLock
   int shareSeg;
#ifdef ANDROID_PLATFORM
   shareSeg = ashmem_create_region(("shmFather" + m_pidString).c_str(),sizeof(sem_t));
   if (shareSeg < 0 )
   {
      std::cerr << "Fail to create shared region for father" << std::endl;
      exit(1);
   }
#else
   if ((shareSeg = shm_open(("/shmFather" + m_pidString).c_str(), O_RDWR | O_CREAT, S_IRWXU)) < 0) 
   {
      perror("shm_open");
      exit(1);
   }
   
   if ( ftruncate(shareSeg, sizeof(sem_t)) < 0 ) {
      perror("ftruncate");
      exit(1);
   }
#endif

  if ((m_fatherLock = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,MAP_SHARED, shareSeg, 0)) == MAP_FAILED) 
  {
      perror("mmap");
      exit(1);
   }
   
   if ( sem_init(m_fatherLock, 1, 0) != 0 )
   {
      std::cerr << "Fail to create semaphore, process will not be synced" << std::endl;
      perror("sem_init");
   }
   
   // open processLock
#ifdef ANDROID_PLATFORM
   shareSeg = ashmem_create_region(("shmProcess" + m_pidString).c_str(),sizeof(sem_t));
   if (shareSeg < 0 )
   {
      std::cerr << "Fail to create shared region for process" << std::endl;
      exit(1);
   }
#else
   // open processLock
   if ((shareSeg = shm_open(("/shmProcess" + m_pidString).c_str(), O_RDWR | O_CREAT, S_IRWXU)) < 0) 
   {
      perror("shm_open");
      exit(1);
   }

  if ( ftruncate(shareSeg, sizeof(sem_t)) < 0 ) {
    perror("ftruncate");
    exit(1);
  }
#endif

  if ((m_processLock = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,MAP_SHARED, shareSeg, 0)) == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }
   
   if ( sem_init(m_processLock, 1, 0) != 0 )
   {
      std::cerr << "Fail to create semaphore, process will not be synced" << std::endl;
      perror("sem_init");
   }

   // open processEndLock
#ifdef ANDROID_PLATFORM
   shareSeg = ashmem_create_region(("shmProcessEnd" + m_pidString).c_str(),sizeof(sem_t));
   if (shareSeg < 0 )
   {
      std::cerr << "Fail to create shared region for process ending" << std::endl;
      exit(1);
   }
#else
   // open processEndLock
   if ((shareSeg = shm_open(("/shmProcessEnd" + m_pidString).c_str(), O_RDWR | O_CREAT, S_IRWXU)) < 0) 
   {
      perror("shm_open");
      exit(1);
   }

  if ( ftruncate(shareSeg, sizeof(sem_t)) < 0 ) {
    perror("ftruncate");
    exit(1);
  }
#endif

  if ((m_processEndLock = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, shareSeg, 0)) == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }
   
   if ( sem_init(m_processEndLock, 1, 0) != 0 )
   {
      std::cerr << "Fail to create semaphore, process will not be synced" << std::endl;
      perror("sem_init");
   }
}

Runner::~Runner()
{
    sem_destroy(m_processEndLock);
#ifndef ANDROID_PLATFORM
    shm_unlink(("/shmProcessEnd" + m_pidString).c_str());
#endif

    sem_destroy(m_processLock);
#ifndef ANDROID_PLATFORM
    shm_unlink(("/shmProcess" + m_pidString).c_str());
#endif

    sem_destroy(m_fatherLock);
#ifndef ANDROID_PLATFORM
    shm_unlink(("/shmFather" + m_pidString).c_str());
#endif
}
