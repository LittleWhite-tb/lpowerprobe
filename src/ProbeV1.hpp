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

#ifndef PROBEV1_HPP
#define PROBEV1_HPP

#include <string>
#include <vector>

#include "Probe.hpp"

/**
 * Wrapper around dynamic evaluation library version 1
 */
class ProbeV1 : public Probe
{
   typedef double (*libGet)(void *data);
   
private:

   libGet evaluationStart;
   libGet evaluationStop;

   double m_startValue;
   double m_measureValue;

public:
   /**
    * Loads a probe from \a path
    * \param path the path to the file to load
    * \exception ProbeLoadingException on failure
    */
   ProbeV1(const std::string& path);
   
   /**
    */
   ~ProbeV1() {}


   void startMeasure();
   double* stopMeasure();
};



#endif
