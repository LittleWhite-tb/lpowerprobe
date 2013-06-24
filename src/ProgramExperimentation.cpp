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

#include "ProgramExperimentation.hpp"

#include <iostream>
#include <sstream>

#include <cstdlib>
#include <cstdio>
#include <exception>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "Kernel.hpp"
#include "ProgramRunner.hpp"
#include "CPUUtils.hpp"

ProgramExperimentation::ProgramExperimentation(const Options &options)
    :Experimentation(options)
{

}

void ProgramExperimentation::start()
{
    // Gets the options to run the test
    const std::vector<std::string>& args = m_options.getArgs();

    unsigned int nbRepet(m_options.getNbRepetition());
    unsigned int nbProcess(m_options.getNbProcess());
    std::vector<unsigned int> pinning(m_options.getPinning());

    ProgramRunner run(m_pProbeDataCollector, m_execFile, args, nbProcess, m_options.getNbMetaRepetition());
    std::vector<pid_t> m_pids;

    for ( unsigned int repet = 0 ; repet < nbRepet ; repet++ )
    {
       for ( unsigned int process = 0 ; process < nbProcess ; process++ )
       {
          pid_t pid = fork();
          if ( pid == 0 ) // Son
          {
             if ( pinning.size() != 0 )
             {
                // Pin it
                CPUUtils::pinCPU(pinning[process]);
             }
             CPUUtils::setFifoMaxPriority(-1);

             run.start(m_pOverheadResults,m_pResults,process);
             if ( process == 0 )
             {
                 saveResults();
             }

             // Need to finish the son here
             _exit(EXIT_SUCCESS);
          }
          else if ( pid > 0 ) // Father
          {
             m_pids.push_back(pid);
          }
          else
          {
             std::cerr << "Fail to create subprocess" << std::endl;
             perror("fork");
             return;
          }
       }

       // Wait for all the child to finish
       for (unsigned int i = 0 ; i < nbProcess ; i++)
       {
          int status=0;

          waitpid (m_pids[i], &status, WUNTRACED | WCONTINUED);
          int res = WEXITSTATUS(status);
          if(res != EXIT_SUCCESS)
          {
             std::cerr << "Child exited with status " << res << ", an error occured.\n" << std::endl;
          }
          if (WIFEXITED(status) == 0)
          {
             char buf[512];

             snprintf (buf, 512, "Error: Child %d received a signal ", i);
             psignal (WTERMSIG (status), buf);
          }
       }
    }

    // saveResults();
}
