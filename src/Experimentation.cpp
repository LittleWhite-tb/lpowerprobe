#include "Experimentation.hpp"

#include <iostream>
#include <sstream>

#include <cstdlib>
#include <cstdio>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "Runner.hpp"
#include "CPUUtils.hpp"

Experimentation::Experimentation(unsigned int nbProcess, const std::vector<unsigned int>& pinning, unsigned int nbRepet, unsigned int nbMetaRepet)
   :m_nbProcess(nbProcess),m_pinning(pinning),m_nbRepet(nbRepet),m_nbMetaRepet(nbMetaRepet)
{
   m_probePaths.push_back("probes/energy_snb_msr/energy_msr_snb.so");
   m_probePaths.push_back("probes/wallclock/wallclock.so");
}

Experimentation::~Experimentation()
{
}

void Experimentation::startExperimentation(const std::string& test, const std::vector<std::string>& args)
{ 
   std::vector<pid_t> m_pids;
   Runner run(m_probePaths,"./output.csv",m_nbProcess,m_nbMetaRepet);
   
   for ( unsigned int repet = 0 ; repet < m_nbRepet ; repet++ )
   {
      for ( unsigned int process = 0 ; process < m_nbProcess ; process++ )
      {
         pid_t pid = fork();
         if ( pid == 0 ) // Son
         {
            if ( m_pinning.size() != 0 )
            {
               // Pin it
               CPUUtils::pinCPU(m_pinning[process]);
            }
            CPUUtils::setFifoMaxPriority(-2);
            
            run.start(test, args, process);
            
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
      for (unsigned int i = 0 ; i < m_nbProcess ; i++)
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
}
