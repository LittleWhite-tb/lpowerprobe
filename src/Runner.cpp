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

#include "CPUUtils.hpp" 

Runner::Runner(const std::vector<std::string>& probePaths, const std::string& resultFileName, unsigned int nbProcess, unsigned int nbMetaRepet)
   :m_resultFile(resultFileName.c_str()),m_nbMetaRepet(nbMetaRepet),m_nbProcess(nbProcess)
{
   for ( std::vector<std::string>::const_iterator itPath = probePaths.begin() ; itPath != probePaths.end() ; ++itPath )
   {
      m_probes.push_back(new Probe(*itPath));
   }
   
   m_overheadResults.resize(m_nbProcess, std::vector< std::vector < std::pair<double, double> > >(m_nbMetaRepet, std::vector<std::pair<double, double> >(m_probes.size(),std::pair<double, double>(0,0))));
   m_runResults.resize(m_nbProcess, std::vector< std::vector < std::pair<double, double> > >(m_nbMetaRepet, std::vector<std::pair<double, double> >(m_probes.size(),std::pair<double, double>(0,0))));
   
   
   int shareSeg;
   if ((shareSeg = shm_open("shmFather", O_RDWR | O_CREAT, S_IRWXU)) < 0) 
   {
      perror("shm_open");
      exit(1);
   }

  if ( ftruncate(shareSeg, sizeof(sem_t)) < 0 ) {
    perror("ftruncate");
    exit(1);
  }

  if ((m_fatherLock = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,MAP_SHARED, shareSeg, 0)) == MAP_FAILED) 
  {
      perror("mmap");
      exit(1);
   }
   
   if ( sem_init(m_fatherLock,1,0) != 0 )
   {
      std::cerr << "Fail to create semaphore, process will not be synced" << std::endl;
      perror("sem_init");
   }
   
   
   if ((shareSeg = shm_open("shmProcess", O_RDWR | O_CREAT, S_IRWXU)) < 0) 
   {
      perror("shm_open");
      exit(1);
   }

  if ( ftruncate(shareSeg, sizeof(sem_t)) < 0 ) {
    perror("ftruncate");
    exit(1);
  }

  if ((m_processLock = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,MAP_SHARED, shareSeg, 0)) == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }
   
   if ( sem_init(m_processLock,2,0) != 0 )
   {
      std::cerr << "Fail to create semaphore, process will not be synced" << std::endl;
      perror("sem_init");
   }
}

Runner::~Runner()
{
   for ( ProbeList::const_iterator itProbe = m_probes.begin() ; itProbe != m_probes.end() ; ++itProbe )
   {
      delete *itProbe;
   }
   
   sem_destroy(m_processLock);
   shm_unlink("shmProcess");
   
   sem_destroy(m_fatherLock);
   shm_unlink("shmFather");
}

void Runner::calculateOverhead(unsigned int metaRepet, unsigned int processNumber)
{
   std::vector<std::string> emptyArgs;
   evaluation(m_overheadResults,"./empty/empty", emptyArgs,metaRepet,processNumber);
}

void Runner::evaluation(GlobalResultsArray& resultArray, const std::string& test, const std::vector<std::string>& args, unsigned int metaRepet, unsigned int processNumber)
{
   bool broken = false;
   int i = 0;
   
   int nbArgs=0;
   char** pArgv = NULL;
         
   /* If argv == NULL, we have to create an argv table anyway... */
   if (args.size() == 0)
   {
      nbArgs = 1;
      pArgv = new char*[nbArgs+1];
      pArgv[0] = strdup(test.c_str());
      pArgv[1] = NULL;
   }
   /* Else, simply execute it with the argv table we got... */
   else
   {
      nbArgs = args.size();
      pArgv = new char*[nbArgs+2];
      pArgv[0] = strdup(test.c_str());
      
      size_t i = 1;
      for ( i = 1 ; i-1 < args.size()  ; i++ )
      {
         pArgv[i] = strdup(args[i-1].c_str());
      }
      pArgv[i] = NULL;
   }
   
   do
   {
      broken = false;
      for (i = 0; i < m_probes.size() ; i++) /* Eval Start */
      {
         resultArray[processNumber][metaRepet][i].first = m_probes[i]->startMeasure();
      }
      
      startTest(test, pArgv, processNumber);
      
      for (i = m_probes.size()-1 ; i >= 0; i--) /* Eval Stop */
      {
         resultArray[processNumber][metaRepet][i].second = m_probes[i]->stopMeasure();
         
         if ( resultArray[processNumber][metaRepet][i].second - resultArray[processNumber][metaRepet][i].first < 0 )
         {
            broken = true;
         }
      }   
   }while(broken);
   
   for (unsigned int i = 0 ; i < nbArgs+1 ; i++ )
   {
      free(pArgv[i]);
   }
   delete [] pArgv;
}

void Runner::startTest(const std::string& programName, char** pArgv, unsigned int processNumber)
{
   pid_t pid = -1;
   switch (pid = fork ())
	{
		case -1:
		{
			exit (EXIT_FAILURE);
		}
		case 0:
		{
         CPUUtils::setFifoMaxPriority(-1);
         if ( sem_post(m_fatherLock) != 0 )
         {
            perror("sem_post");
         }
         
         if ( sem_wait(m_processLock) != 0 )
         {
            perror("sem_wait");
         }
         
         execvp (programName.c_str(), pArgv);
         exit(EXIT_SUCCESS);
      }
      default:
		{
			// Father
			int status;
         
         if ( processNumber == 0 )
         {
            for ( unsigned int i = 0 ; i < m_nbProcess  ; i++ )
            {
               if ( sem_wait(m_fatherLock) != 0 )
               {
                  perror("sem_wait father");
               }
               
               if ( sem_post(m_processLock) != 0 )
               {
                  perror("sem_post process");
               }
            }
         }
         
			waitpid (pid, &status, 0);
			
			int res = WEXITSTATUS (status);
			/* Child must end normally */
			if (WIFEXITED (status) == 0)
			{
				if (WIFSIGNALED (status))
				{
					 psignal (WTERMSIG (status), "Error: Input benchmark performed an error, exiting now...");
					 exit (EXIT_FAILURE);
				}
				std::cerr << "Benchmark ended non-normally, exiting now..." << std::endl;
				exit (EXIT_FAILURE);
			}
         
			break;
		}
   }
}

void Runner::saveResults()
{
   // We save only the results for the first process
   std::vector<double> libsOverheadAvg(m_probes.size());
   for ( std::vector<std::vector<std::pair<double, double> > >::const_iterator itMRepet = m_overheadResults[0].begin() ;
         itMRepet != m_overheadResults[0].end() ; ++itMRepet )
   {
      for ( unsigned int i = 0 ; i < itMRepet->size() ; i++ )
      {
         libsOverheadAvg[i] += (*itMRepet)[i].second - (*itMRepet)[i].first;
      }
   }
   
   for ( std::vector<double>::iterator itLib = libsOverheadAvg.begin() ; itLib != libsOverheadAvg.end() ; ++itLib )
   {
      *itLib /= m_overheadResults[0].size();
   }
   
   std::vector< std::vector<double> > runResults(m_runResults[0].size(),std::vector<double>(m_probes.size(),0));
   for ( unsigned int mRepet = 0 ; mRepet < m_runResults[0].size() ; mRepet++ )
   {
      for ( unsigned int i = 0 ; i < m_runResults[0][mRepet].size() ; i++ )
      {
         runResults[mRepet][i] += (m_runResults[0][mRepet][i].second - m_runResults[0][mRepet][i].first) - libsOverheadAvg[i];
         
         m_resultFile << runResults[mRepet][i];
         if ( i !=  m_runResults[0][mRepet].size()-1 )
         {
            m_resultFile << ";";
         }
         else
         {
            m_resultFile << std::endl;
         }
      }
   }
}

void Runner::start(const std::string& test, const std::vector<std::string>& args, unsigned int processNumber)
{
   for (unsigned int metaRepet = 0; metaRepet < m_nbMetaRepet ; metaRepet++)
	{
      calculateOverhead(metaRepet,processNumber);
   }
   
   for (unsigned int metaRepet = 0; metaRepet < m_nbMetaRepet ; metaRepet++)
	{
      if ( processNumber == 0 )
      {
         std::cout << "Repetition - " << metaRepet << std::endl;
      }
      evaluation(m_runResults,test,args,metaRepet,processNumber);
   }
   
   if ( processNumber == 0 )
   {
      saveResults();
   }
}
