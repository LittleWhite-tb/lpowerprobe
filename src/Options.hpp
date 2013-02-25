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

#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <vector>
#include <string>

/**
 * Container for program options
 * The method \a setDefaultValues() sets the default values to unset options
 * then, you can check with \a hasMissingOptions() function if all the needed
 * options have been specified.
 */
class Options
{
private:

   unsigned int m_nbProcess;
   std::vector<unsigned int> m_pinning;
   
   unsigned int m_nbRepet;
   unsigned int m_nbMetaRepet;
   
   std::string m_execName;
   std::vector<std::string> m_args;
   
   /**
    * Default values
    */
   static const unsigned int DEFAULT_NUMBER_PROCESS = 1;
   static const unsigned int DEFAULT_NUMBER_REPET = 1;
   static const unsigned int DEFAULT_NUMBER_METAREPET = 5;
   
   
   /**
    * Helper to test if a function is set a second time (override) or not
    * If it is the case, we are outputting a warning
    * \param oldValue the previous option value
    * \param newValue the value to set
    * \param optionName the litteral name of the option (used in the warning message)
    */
   void checkOverride(unsigned int oldValue, unsigned int newValue, const std::string& optionName);

public:
   /**
    */
   Options();
   
   /**
    */
   ~Options();
   
   /**
    * \return the number of process to create
    */
   unsigned int getNbProcess()const;
   
   /**
    * Sets the number of process to create
    * \param numberProcess
    */
   void setNbProcess(unsigned int numberProcess);
   
   /**
    * \return the number of process to create
    */
   const std::vector<unsigned int>& getPinning()const;
   
   /**
    * \param pinning the pinning to apply to process
    */
   bool setPinning(const std::string& pinning);
   
   /**
    * \return the number of repetition to do
    */
   unsigned int getNbRepetition()const;
   
   /**
    * \param numberRepetition the number of repetition to run
    */
   void setNbRepetition(unsigned int numberRepetition);
   
   /**
    * \return the number of meta repetition to do
    */
   unsigned int getNbMetaRepetition()const;
   
   /**
    * \param numberMetaRepetition the number of meta repetition to run
    */
   void setNbMetaRepetition(unsigned int numberMetaRepetition);
   
   /**
    * \return the test name
    */
   const std::string& getExecName()const;
   
   /**
    * Set the test name to run
    * \param pExecName
    */
   void setExecName(const char* pExecName);
   
   /**
    * \return the list of args to pass to the test
    */
   const std::vector<std::string>& getArgs()const;
   
   /**
    * Add an argument to the list of arguments to pass to the test
    * \param pArg the new argument
    */
   void addArg(const char* pArg);
   
   /**
    * Set the default values to unset options
    */
   void setDefaultValues();
   
   /**
    * Check if all necessary options have been set
    * \return true if there is a missing options
    */
   bool hasMissingOptions();
};

#endif
