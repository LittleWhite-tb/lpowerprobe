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

#ifndef EXPERIMENTATION_HPP
#define EXPERIMENTATION_HPP

#include <vector>
#include <string>

class Options;

/**
 * Set up the experimentation context and start the benchmarking using \a Runner
 * The experimentation is following the following path :
 * - Looping for all experimentation
 * - - Looping for all process
 * - - - Creating fork
 * - - - Looping for all metarepetition (Runner)
 * - - - Start test program (Runner)
 * 
 * The forks gets an higher priority and can be pinned 
 */
class Experimentation
{
private:
   static const std::string DUMMY_KERNEL_FILE;

   const Options& m_options;
   std::string m_execFile;
   
   std::string m_dummyKernelFile;

   std::vector<std::string> m_probePaths; /*!< List of probes to load */
   
   void startProgramExperimentation();
   void startKernelExperimentation();
   
public:
   /**
    * \param options the options given by the user
    */
   Experimentation(const Options& options);
   
   /**
    */
   ~Experimentation();
   
   /**
    * Starts the experimentation process
    * \param test the program to bench
    * \param args the arguments to pass to the program
    */
   void startExperimentation();
};

#endif
