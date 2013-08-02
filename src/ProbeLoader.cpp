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

#include "ProbeLoadingException.hpp"
#include "ProbeInitialisationException.hpp"
#include "InvalidProbeVersionException.hpp"

#include "ProbeV1.hpp"
#include "ProbeV2.hpp"

#include <iostream>

ProbeLoader::ProbeLoader()
{
   // Don't forget to end path with '/'
   m_dirs.push_back(INSTALL_DIR "/lib/lPowerProbe/");
   m_dirs.push_back("");
   m_dirs.push_back("./");
   m_dirs.push_back("./probes/");
   
   m_defaultsProbes.push_back("libenergymsr/libenergymsr.so");
   m_defaultsProbes.push_back("libwallclock/libwallclock.so");
}

void ProbeLoader::tryLoadProbe(const std::string& probePath, ProbeList& probes)
{
    Probe* pProbe = NULL;
    try
    {
       pProbe = new ProbeV2(probePath);

    }
    catch (InvalidProbeVersionException& ple)
    {
       // Now, we try to load V1
        pProbe = new ProbeV1(probePath);
    }
    probes.push_back(pProbe);
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
      bool loopStopper = false;
      for ( std::vector<std::string>::const_iterator itDir = m_dirs.begin() ; itDir != m_dirs.end() && loaded == false && loopStopper == false ; ++itDir )
      {
         try
         {
            tryLoadProbe(*itDir + *itProbePath,probes);
            loaded = true; // stops the loop
         }
         catch (ProbeLoadingException& ple)
         {
            errors.push_back(ple.what());
         }
          catch (ProbeInitialisationException& pie)
          {
              // we don't bother about old 'not found' errors, since we found it, but not able to init it
             errors.clear();
             errors.push_back(pie.what());
             loopStopper = true;
          }
      }
      
      if ( loaded == false )
      {
         std::cout << "ERROR to load probe '" << *itProbePath << "'" << std::endl;
         for ( std::vector<std::string>::const_iterator itError = errors.begin() ; itError != errors.end() ; ++itError )
         {
            std::cout << "==> " << *itError << std::endl;
         }
         std::cout << "Failed" << std::endl;
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
