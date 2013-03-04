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
#include <sstream>
#include <cstdlib>

#include <getopt.h>

#include "Experimentation.hpp"
#include "Options.hpp"

const int ID_ARG_REPETITION = 1;
const int ID_ARG_METAREPETITION = 2;
const int ID_ARG_PROCESS = 3;
const int ID_ARG_PINNING = 4;
const int ID_ARG_OUTPUT = 5;

static struct option long_options[] =
{
   {"help", no_argument,   0, 'h' },
   {"repetition", required_argument,   0, ID_ARG_REPETITION},
   // {"meta-repetition", required_argument,   0, ID_ARG_METAREPETITION},
   {"duplicate", required_argument,   0, ID_ARG_PROCESS},
   {"pinning", required_argument,   0, ID_ARG_PINNING},
   {"output", required_argument,   0, ID_ARG_OUTPUT},
   {0,0,0,0}
};

template<typename T>
bool from_string( const std::string & Str, T & Dest )
{
    // créer un flux à partir de la chaîne donnée
    std::istringstream iss( Str );
    // tenter la conversion vers Dest
    return iss >> Dest != 0;
}

void usage()
{
   std::cout << "./lPowerProbe [OPTION] prog args" << std::endl;
   std::cout << "  -h, --help\t\t\t\tDisplay this help" << std::endl;
   std::cout << "  -r, --repetition=NUMBER\t\tNumber of repetition for the test" << std::endl;
   // std::cout << "      --meta-repetition=NUMBER\t\tNumber of meta repetition for the test" << std::endl;
   std::cout << "  -d, --duplicate=NUMBER\t\tNumber of process to start" << std::endl;
   std::cout << "  -p, --pinning=\"pin1;pin2;...\"\tWhere to pin the processes" << std::endl;
   std::cout << "  -o, --output=\"resultFile\"\tWhere to write the results" << std::endl;
   exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
   int flags, opt=0;
   int option_index = 0;
   Options options;
   
   while(1)
   {
      opt = getopt_long(argc, argv, "r:d:p:o:h", long_options, &option_index);
      if (opt == -1 )
         break;
      
      
      std::cout << "Opt : " << opt;
      if ( optarg )
      {
         std::cout << " with '" << optarg << std::endl;
      }
      
      switch (opt)
      {
         case ID_ARG_REPETITION:
         case 'r':
            {
               unsigned int repet = 0;
               if ( from_string<unsigned int>(optarg,repet) )
               {
                  // Hack to replace the repetition command to meta repetition
                  options.setNbMetaRepetition(repet);
               }
               else
               {
                  std::cerr << "Invalid argument for --repetition option" << std::endl;
                  usage();
               }
            }
            break;
         case ID_ARG_METAREPETITION:
            {
               unsigned int mrepet = 0;
               if ( from_string<unsigned int>(optarg,mrepet) )
               {
                  options.setNbMetaRepetition(mrepet);
               }
               else
               {
                  std::cerr << "Invalid argument for --meta-repetition option" << std::endl;
                  usage();
               }
            }
            break;
         case ID_ARG_PROCESS:
         case 'd':
            {
               unsigned int process = 0;
               if ( from_string<unsigned int>(optarg,process) )
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
         case 'h':
            usage();
            break;
         case '?':
            break;
      }
   }
   
   if (optind < argc)
   {
      int i = 0;
      while (optind < argc)
      {
         if ( i == 0 )
         {
            std::cout << "Test program : '" << argv[optind] << "'" << std::endl; 
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
   
   Experimentation e(options.getNbProcess(), options.getPinning(), options.getNbRepetition(), options.getNbMetaRepetition());
   e.startExperimentation(options.getExecName(),options.getArgs(),options.getOutputFile());
   
   return 0;
}
