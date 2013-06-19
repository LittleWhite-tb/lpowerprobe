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

#ifndef PROBEINITIALISATIONEXCEPTION_HPP
#define PROBEINITIALISATIONEXCEPTION_HPP

#include <exception>

/**
 * @class ProbeInitialisationException
 * @brief Thrown when a probe failed to initialise
 */
class ProbeInitialisationException: public std::exception
{
   public:

      /**
       * \param libName the name of the probe that failed
       */
      ProbeInitialisationException(const std::string& libName)
         :libName(libName)
      {
      }

      virtual ~ProbeInitialisationException(void)throw() {}

      /**
       * \return the litteral reason of the exception
       */
      virtual const char* what() const throw()
      {
          return (std::string("Probe ") + libName + std::string(" failed to init")).c_str();
      }

   private:
      std::string libName;
};

#endif
