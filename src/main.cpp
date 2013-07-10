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

#include <iostream>
#include <cstdlib>

#include <getopt.h>
#include <cstdio>
#include <cerrno>
#include <cassert>
#include <sys/stat.h>

#include "StringUtils.hpp"

#include "ExperimentationFactory.hpp"
#include "Experimentation.hpp"
#include "Options.hpp"

#include "version.hpp"

enum e_idarg {
   ID_ARG_REPETITION,
   ID_ARG_PROCESS,
   ID_ARG_PINNING,
   ID_ARG_OUTPUT,
   ID_ARG_MEMSIZE,
   ID_ARG_KERNELITER,
   ID_ARG_PROBES
};

static struct option long_options[] =
{
   {"help", no_argument,   0, 'h' },
   {"version", no_argument,   0, 'v' },
   {"repetition", required_argument,   0, ID_ARG_REPETITION},
   {"duplicate", required_argument,   0, ID_ARG_PROCESS},
   {"pinning", required_argument,   0, ID_ARG_PINNING},
   {"output", required_argument,   0, ID_ARG_OUTPUT},
   {"iteration-mem-size", required_argument,   0, ID_ARG_MEMSIZE},
   {"iteration", required_argument,   0, ID_ARG_KERNELITER},
   {"libraries", required_argument,   0, ID_ARG_PROBES},
   {0,0,0,0}
};

void version()
{
   std::cout << "./lProwerProbe © Universite de Versailles 2013" << std::endl;
   std::cout << "\t\tversion : " + std::string(GIT_HASH) + " (" + std::string(GIT_COUNT) + ")" << std::endl;
   std::cout << std::endl;
}

void usage()
{
   std::cout << "./lPowerProbe [OPTION] [prog] [args]" << std::endl;
   std::cout << std::endl;
   std::cout << "Performs standardized and accurate measurements." << std::endl;
   std::cout << "It can work in three different modes depending on the prog argument value:" << std::endl;
   std::cout << "\t- prog is not specified: the measurement is (re)started/ended when receiving SIGUSR1." << std::endl;
   std::cout << "\t- prog ends by .s: it is compiled and run instrumented as a kernel." << std::endl;
   std::cout << "\t- otherwise: prog is run instrumented as an executable file." << std::endl;
   std::cout << "  -h, --help\t\t\t\tDisplay this help" << std::endl;
   std::cout << "  -v, --version\t\t\t\tDisplay the program version" << std::endl;
   
   std::cout << std::endl;
   
   std::cout << "  -l, --libraries\t\t\tProbes libraries to load to bench the execution" << std::endl;
   std::cout << "  -r, --repetition=NUMBER\t\tNumber of repetition for the test" << std::endl;
   // std::cout << "      --meta-repetition=NUMBER\t\tNumber of meta repetition for the test" << std::endl;
   std::cout << "  -d, --duplicate=NUMBER\t\tNumber of process to start" << std::endl;
   std::cout << "  -p, --pinning=\"pin1;pin2;...\"\t\tWhere to pin the processes" << std::endl;
   std::cout << "  -o, --output=\"resultFile\"\t\tWhere to write the results" << std::endl;
   std::cout << "  -m, --iteration-mem-size=NUMBER\tThe memory size in bytes used in the kernel per iteration" << std::endl;
   std::cout << "  -i, --iteration=NUMBER\t\tThe number of kernel iteration" << std::endl;
   exit(EXIT_SUCCESS);
}

bool isFileExecutable(const char* pPath)
{
   assert(pPath);
   
   struct stat fileInfo;
   
   if (stat(pPath,&fileInfo) != 0 )
   {
      std::cout << "Warning : Fail to stat the file '" << pPath << "'" << std::endl;
      perror("");
      std::cout << "Info : since the executable you want to run can be in PATH, we will continue" << std::endl;
      return false;
   }
   
   if ((fileInfo.st_mode & S_IEXEC) != 0 )
   {
      return true;
   }
   
   std::cout << "The executable specified '" << pPath << "' is not executable" << std::endl;
   return false;
}

int main(int argc, char** argv)
{
   int opt=0;
   int option_index = 0;
   Options options;
   
   while(1)
   {
      opt = getopt_long(argc, argv, "r:d:p:o:m:i:l:hv", long_options, &option_index);
      if (opt == -1 )
         break;
      
      
      switch (opt)
      {
         case ID_ARG_REPETITION:
         case 'r':
            {
               unsigned int repet = 0;
               if ( StringUtils::from_string<unsigned int>(optarg,repet) )
               {
                  // Hack to replace the repetition command to meta repetition
                  options.setNbRepetition(repet);
               }
               else
               {
                  std::cerr << "Invalid argument for --repetition option" << std::endl;
                  usage();
               }
            }
            break;
         case ID_ARG_PROCESS:
         case 'd':
            {
               unsigned int process = 0;
               if ( StringUtils::from_string<unsigned int>(optarg,process) )
               {
                  options.setNbProcess(process);
               }
               else
               {
                  std::cerr << "Invalid argument for --process option" << std::endl;
                  usage();
               }
            }
            break;
         case ID_ARG_PINNING:
         case 'p':
            {
               if ( !options.setPinning(optarg) )
               {
                  std::cerr << "Invalid argument for --pinning option" << std::endl;
                  usage();
               }
            }
            break;
         case ID_ARG_OUTPUT:
         case 'o':
            {
               options.setOutputFile(optarg);
            }
            break;
            
         case ID_ARG_MEMSIZE:
         case 'm':
            {
               size_t iterationMemorySize = 0;
               if ( StringUtils::from_string<size_t>(optarg,iterationMemorySize) )
               {
                  options.setIterationMemorySize(iterationMemorySize);
               }
               else
               {
                  std::cerr << "Invalid argument for --iteration-mem-size option" << std::endl;
                  usage();
               }
            }
            break;
         
         case ID_ARG_KERNELITER:
         case 'i':
            {
               size_t nbKernelIteration = 0;
               if ( StringUtils::from_string<unsigned long int>(optarg,nbKernelIteration) )
               {
                  options.setNbKernelIteration(nbKernelIteration);
               }
               else
               {
                  std::cerr << "Invalid argument for --kernel-iteration option" << std::endl;
                  usage();
               }
            }
            break;
         case ID_ARG_PROBES:
         case 'l':
            {
               if ( !options.setProbesPath(optarg) )
               {
                  std::cerr << "Invalid argument for --probes option" << std::endl;
                  usage();
               }
            }
            break;
         case 'h':
            usage();
            break;
         case 'v':
            version();
            exit(EXIT_SUCCESS);
            break;
         case '?':
            break;
      }
   }
   
   version();
   
   if (optind < argc)
   {
      int i = 0;
      while (optind < argc)
      {
         if ( i == 0 )
         {
            std::cout << "Test program : '" << argv[optind] << "'" << std::endl; 
            
            isFileExecutable(argv[optind]);

            options.setExecName(argv[optind]);
         }
         else
         {
            options.addArg(argv[optind]);
         }
         optind++;
         i++;
      }
   }
   
   options.setDefaultValues();
   if ( options.hasMissingOptions() )
   {
      std::cerr << "Some options are mandatory" << std::endl;
      usage();
   }
   

   Experimentation* pExp = ExperimentationFactory::createExperimentation(options);
   pExp->start();
   delete pExp;

   return 0;
}
