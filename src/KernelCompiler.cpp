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

#include "KernelCompiler.hpp"

#include <iostream>
#include <cstdio>
#include <cstdlib>

#include <unistd.h>
#include <errno.h>

// TODO : At some point, we have to move this function in a specific file
std::string traceSystemCommand(const std::string& command, int& result)
{
   char buf[200];
   std::string output;
   
   FILE* pStream = popen(command.c_str(),"r");
   if ( pStream == NULL )
   {
      std::cerr << "Failed to trace command '" << command << "'" << std::endl;
      return "";
   }
   
   while(fgets(buf, sizeof(buf), pStream))
   {
      output += buf;
   }
   
   result = pclose(pStream);
   
   return output;
}

bool KernelCompiler::compile(const std::string& inputFile, std::string& outputFile)
{
   int res = 0;
   char outputName[] = "/tmp/lPowerProbe_kernel_XXXXXX";
   
   // Create a temporary file to lock this name in order to give the ability to GCC 
   // to generate the kernel
   res = mkstemp (outputName);
   if (res == -1)
   {
      perror ("Cannot create temporary file for compilation ");
      return false;
   }
   close(res); // Opened by mkstemp, but we don't need it. We have a proper tmp file name for us
   
   std::string gccCommand = std::string("gcc -fPIC -O3 -Wall -Wextra -shared -Wl,-soname,") + outputName + " -o " + outputName + " " + inputFile;

   std::string commandOutput = traceSystemCommand(gccCommand,res);
   if ( res != 0 )
   {
      std::cerr << "Failed to compile the kernel :" << std::endl;
      std::cerr << commandOutput << std::endl;
      return false;
   }
   
   outputFile = outputName;
   return true;
}
