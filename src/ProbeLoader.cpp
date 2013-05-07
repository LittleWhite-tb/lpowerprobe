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

#include "ProbeLoader.hpp"

#include <iostream>

ProbeLoader::ProbeLoader()
{
   // Don't forget to end path with '/'
   m_dirs.push_back(INSTALL_DIR "/lib/lPowerProbe/");
   m_dirs.push_back("");
   m_dirs.push_back("./");
   m_dirs.push_back("./probes/");
   
   m_defaultsProbes.push_back("energy_msr_snb.so");
   m_defaultsProbes.push_back("wallclock.so");
}

bool ProbeLoader::tryLoadProbes(const std::vector<std::string>& probesPath, ProbeList& probes)
{
   std::vector<std::string> errors;
   bool result = true;
   
   // User specified libs to load
   for ( std::vector<std::string>::const_iterator itProbePath = probesPath.begin() ; itProbePath != probesPath.end() ; ++itProbePath )
   {
      errors.clear();
      bool loaded = false;
      std::cout << "Trying to load : " << *itProbePath << " ... ";
      for ( std::vector<std::string>::const_iterator itDir = m_dirs.begin() ; itDir != m_dirs.end() && loaded == false ; ++itDir )
      {
         try
         {
            Probe* pProbe = new Probe(*itDir + *itProbePath);
            probes.push_back(pProbe);
            loaded = true; // stops the loop
         }
         catch (ProbeLoadingException& ple)
         {
            errors.push_back(ple.what());
         }
      }
      
      if ( loaded == false )
      {
         std::cout << "Failed" << std::endl;
         for ( std::vector<std::string>::const_iterator itError = errors.begin() ; itError != errors.end() ; ++itError )
         {
            std::cout << "==> " << *itError << std::endl;
         }
         result = false;
      }
   }
   
   return result;
}
      
bool ProbeLoader::loadProbes(const std::vector<std::string>& probesPath, ProbeList& probes)
{
   // No probes specified by user, try to load defaults one
   if ( probesPath.size() == 0 )
   {
      std::cout << "No probes specified, loading defaults" << std::endl;
      return tryLoadProbes(m_defaultsProbes,probes);
   }
   else
   {
      return tryLoadProbes(probesPath,probes);
   }
}
