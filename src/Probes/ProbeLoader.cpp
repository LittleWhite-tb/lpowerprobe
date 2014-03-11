/*
 * lPowerProbe - A light benchmark tool oriented for energy probing
 *               heavely based on likwid
 * Copyright (C) 2013-2014 Universite de Versailles
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
    catch (ProbeLoadingException& ple) {
        pProbe = new ProbeV1 (probePath);
    }
    probes.push_back(pProbe);
}

bool ProbeLoader::tryLoadProbes(const std::vector<std::string>& probesPath, ProbeList& probes)
{
   bool result = true;
   
   // User specified libs to load
   for ( std::vector<std::string>::const_iterator itProbePath = probesPath.begin() ; itProbePath != probesPath.end() ; ++itProbePath )
   {
      std::string error;
      try {
         tryLoadProbe(*itProbePath, probes);
      }
      catch (std::exception &ple) {
         error = ple.what();

         std::cout << "ERROR while loading probe '" << *itProbePath << "': " << error << std::endl;
         result = false;
      }
   }
   
   return result;
}

bool ProbeLoader::loadProbes(const std::vector<std::string>& probesPath, ProbeList& probes)
{
   // No probes specified by user, try to load defaults one
   std::cout << std::endl << std::endl;
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
