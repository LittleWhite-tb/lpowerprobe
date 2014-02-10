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

#ifndef KERNELCOMPILER_HPP
#define KERNELCOMPILER_HPP

#include <string>

/**
 * @class KernelCompilationException
 * @brief Thrown when GCC failed to compile the kernel
 */
class KernelCompilationException: public std::exception
{
   public:
   
      /**
       * \param message reason of the exception
       */
      KernelCompilationException(const std::string& message)
         :message(message)
      {
      }
   
      virtual ~KernelCompilationException(void)throw() {}

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

/**
 * Compiler wrapping for kernel
 * 
 * This compile using GCC a .s (assembly) file in order to create an executable of the kernel
 */
class KernelCompiler
{
   private:
   
   public:
      // Note : could throw exception to handle errors
      /**
       * Compile a kernel specified by \a inputFile
       * The location of the executable will be specified by \a outputFile
       * if the function returns true
       * \param inputFile the kernel to compile
       * \param outputFile the executable location
       * \return true if the compilation is successful
       */
      static bool compile(const std::string& inputFile, std::string& outputFile);
};

#endif
