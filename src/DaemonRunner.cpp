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

#include <csignal>
#include <unistd.h>

#include "DaemonRunner.hpp"

static void sighandler(int sig);

bool DaemonRunner::end = false;

DaemonRunner::DaemonRunner(ProbeList* pProbes, const std::string& resultFileName)
    : Runner(pProbes, resultFileName, 1, 1)
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
}

void sighandler(int sig) {
   if (sig == SIGTERM || sig == SIGINT) {
      DaemonRunner::end = true;
   }
}

void DaemonRunner::start(unsigned int processNumber)
{
   (void) processNumber;

   for (unsigned int metaRepet = 0; !DaemonRunner::end; metaRepet++) {
      m_nbMetaRepet = metaRepet + 1;

      m_runResults.resize(1,
         std::vector< std::vector < std::pair<double, double> > >(m_nbMetaRepet,
            std::vector<std::pair<double, double> >(m_pProbes->size(),
               std::pair<double, double>(0,0))));

      for (size_t i = 0; i < m_pProbes->size(); i++) {
         m_runResults[0][metaRepet][i].first = (*m_pProbes)[i]->startMeasure();
      }

      pause();

      for (int i = m_pProbes->size() - 1; i >= 0; i--) {
         m_runResults[0][metaRepet][i].second = (*m_pProbes)[i]->stopMeasure();
      }
   }

   DaemonRunner::end = true;

   saveResults();
}
