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


#ifndef STRINGUTILS_HPP
#define STRINGUTILS_HPP

#include <sstream>

/**
 * Utility class for string
 * The class offers functions to convert values to string \a to_string() and string to value \a from_string()
 */
class StringUtils
{
    public:

      /**
       * Convert a string to a value
       * \param Str the string from which one the value will be read
       * \param Dest the value got from the string
       * \return false when an error occurs
       */
       template<typename T>
       static bool from_string( const std::string & Str, T & Dest )
       {
          // créer un flux à partir de la chaîne donnée
          std::istringstream iss( Str );
          // tenter la conversion vers Dest
          return iss >> Dest != 0;
       }

       /**
        * Convert a value to a string
        * \param Value the value to convert
        * \param str the string resulting
        * \return false when an error occurs
        */
       template<typename T>
       static bool to_string(const T& Value, std::string& str)
       {
          std::ostringstream oss;
          bool result = oss << Value != 0;

          str = oss.str();
          return result;
       }
};

#endif
