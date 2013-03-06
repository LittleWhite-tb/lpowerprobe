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

   std::vector<std::string> m_probePaths; /*!< List of probes to load */

   unsigned int m_nbProcess;  /*!< Number of process to start */
   std::vector<unsigned int> m_pinning; /*!< Process pinning */
   
   unsigned int m_nbRepet; /*!< Number of repetition */
   unsigned int m_nbMetaRepet; /*!< Number of meta repetition */
   
public:
   /**
    * \param programPath the program path used to run the program the number
    * \param nbProcess the number of process to start
    * \param pinning the process pinning
    * \param nbRepet the number of repetition
    * \param nbMetaRepet the number of meta repetition
    */
   Experimentation(const std::string& programPath, unsigned int nbProcess, const std::vector<unsigned int>& pinning, unsigned int nbRepet, unsigned int nbMetaRepet);
   
   /**
    */
   ~Experimentation();
   
   /**
    * Starts the experimentation process
    * \param test the program to bench
    * \param args the arguments to pass to the program
    */
   void startExperimentation(const std::string& test, const std::vector<std::string>& args, const std::string& outputFile);
};

#endif
