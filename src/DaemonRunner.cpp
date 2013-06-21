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
#include <fstream>
#include <unistd.h>

#include "DaemonRunner.hpp"

static void sighandler(int sig);

bool DaemonRunner::end = false;

#define PID_FILE "/tmp/lppDaemonPID"

DaemonRunner::DaemonRunner(ProbeDataCollector* pProbesDataCollector, const std::string& resultFileName)
    : Runner(pProbesDataCollector, resultFileName, 1, 1)
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
   std::fstream fs(PID_FILE);
   fs << getpid();
   fs.flush();
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
}

void DaemonRunner::start(unsigned int processNumber)
{
   (void) processNumber;

   // compute overhead based on one iteration
   m_overheadResults.resize(1,
         std::vector< std::vector < RunData* > >(1,
            std::vector<RunData*>(m_pProbesDataCollector->getNumberProbes(),
               NULL)));

   m_pProbesDataCollector->start();
   m_pProbesDataCollector->stop();
   for (unsigned int i = 0 ; i < m_pProbesDataCollector->getNumberProbes() ; i++ )
   {
      m_overheadResults[0][0][i] = m_pProbesDataCollector->getData(i);
   }
   m_pProbesDataCollector->clear();
   m_pProbesDataCollector->allocateMemory();

   // running
   for (unsigned int metaRepet = 0; !DaemonRunner::end; metaRepet++) {
      m_nbMetaRepet = metaRepet + 1;

      m_runResults.resize(1,
         std::vector< std::vector < RunData* > >(m_nbMetaRepet,
            std::vector<RunData*>(m_pProbesDataCollector->getNumberProbes(),
               NULL)));

      m_pProbesDataCollector->start();

      pause();

      m_pProbesDataCollector->stop();
      for (unsigned int i = 0 ; i < m_pProbesDataCollector->getNumberProbes() ; i++ )
      {
          m_runResults[0][metaRepet][i] = m_pProbesDataCollector->getData(i);
      }
      m_pProbesDataCollector->clear();
      m_pProbesDataCollector->allocateMemory();
   }

   DaemonRunner::end = true;

   saveResults();
}
