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

#ifndef INVALIDPROBEVERSIONEXCEPTION_HPP
#define INVALIDPROBEVERSIONEXCEPTION_HPP

#include <exception>

/**
 * @class InvalidProbeVersionException
 * @brief Thrown when a probe is not in compatible version
 */
class InvalidProbeVersionException : public std::exception
{
   public:

      /**
       * \param message reason of the exception
       */
      InvalidProbeVersionException(const std::string& message)
         :message(message)
      {
      }

      virtual ~InvalidProbeVersionException(void)throw() {}

      /**
       * \return the littral reason of the exception
       */
      virtual const char* what() const throw()
      {
         return message.c_str();
      }

   private:
      std::string message;
};

#endif
