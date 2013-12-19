/*
 * lPowerProbe - A light benchmark tool oriented for energy probing
 *               heavely based on likwid
 * Copyright (C) 2013 Universite de Versailles
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

#include <cstdio>
#include <csignal>
#include <cstring>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include "DaemonRunner.hpp"

static void sighandler(int sig);

volatile bool DaemonRunner::end = false;

#define PID_FILE "/tmp/lppDaemonPID"

DaemonRunner::DaemonRunner(ProbeDataCollector* pProbesDataCollector)
    : Runner(pProbesDataCollector, 1, 1)
{
   struct sigaction sact;
   sigset_t block_mask;

   sigemptyset (&block_mask);
   
   sact.sa_handler = sighandler;
   sact.sa_mask = block_mask;
   sact.sa_flags = 0;

   sigaction(SIGTERM, &sact, NULL);
   sigaction(SIGINT, &sact, NULL);
   sigaction(SIGUSR1, &sact, NULL);

   // also notify the processes of our pid
   std::fstream fs(PID_FILE, std::fstream::out);
   if (!fs) {
      std::cerr << "Failed to open the PID file." << std::endl;
   } else {
      fs << getpid();
      fs.flush();
   }
   fs.close();
}

DaemonRunner::~DaemonRunner()
{
   struct sigaction sact;
   sigset_t block_mask;

   sigemptyset (&block_mask);

   sact.sa_handler = SIG_DFL;
   sact.sa_mask = block_mask;
   sact.sa_flags = 0;

   sigaction(SIGTERM, &sact, NULL);
   sigaction(SIGINT, &sact, NULL);
   sigaction(SIGUSR1, &sact, NULL);

   remove(PID_FILE);
}

void sighandler(int sig) {
   if (sig == SIGTERM || sig == SIGINT) {
      DaemonRunner::end = true;
   }

   std::cout << "--- Received signal " << strsignal(sig) << std::endl;
}

void DaemonRunner::start(ExperimentationResults* pOverheadExpResult, ExperimentationResults* pExpResult, unsigned int processNumber)
{
   (void) processNumber;

    std::cout << "=== Daemon mode started ===" << std::endl;

   // compute overhead based on one iteration
   m_pProbesDataCollector->start();
   m_pProbesDataCollector->stop(pOverheadExpResult);

   // running
   for (unsigned int metaRepet = 0; !DaemonRunner::end; metaRepet++)
   {
      m_nbMetaRepet = metaRepet + 1;

      pause();

      if (DaemonRunner::end) {
         break;
      }

      std::cout << "--- starting measurement" << std::endl;

      m_pProbesDataCollector->start();

      pause();

      if (DaemonRunner::end) {
         m_pProbesDataCollector->cancel();
      } else {

         std::cout << "--- ending measurement" << std::endl;
         m_pProbesDataCollector->stop(pExpResult);
      }
   }

   DaemonRunner::end = false;

    std::cout << "=== Daemon mode completed ===" << std::endl;
}
