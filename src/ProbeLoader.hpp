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

#ifndef PROBELOADER_HPP
#define PROBELOADER_HPP

#include <vector>
#include <string>

#include "Probe.hpp"

class ProbeLoader
{
   private:
   
      std::vector<std::string> m_dirs; /*!< search directories */
      std::vector<std::string> m_defaultsProbes;   /*!< default libraries to load when non have been specified */

      /**
       * Tries to load a probe (either v1 or v2)
       * @param probePath
       */
      void tryLoadProbe(const std::string& probePath, ProbeList& probes);

      /**
       * Goes throught the directories to load the probes specified by \a probesPath
       * \param probesPath the probes to load
       * \param probes list of probes loaded
       * \return false if at least one probe failed to be loaded
       */
      bool tryLoadProbes(const std::vector<std::string>& probesPath, ProbeList& probes);
   
   public:
   
      /**
       */
      ProbeLoader();
      
      /**
       * Load the probes specified by \a probesPath or the defaults probes when \a probesPath is empty
       * \param probesPath the probes to load
       * \param probes list of probes loaded
       * \return false if at least one probe failed to be loaded
       */
      bool loadProbes(const std::vector<std::string>& probesPath, ProbeList& probes);
};

#endif
