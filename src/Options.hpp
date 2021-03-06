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

#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <vector>
#include <string>

/**
 * Container for program options
 * The method \a setDefaultValues() sets the default values to unset options
 * then, you can check with \a hasMissingOptions() function if all the needed
 * options have been specified.
 * 
 * The me
 */
class Options
{
private:

   std::string m_programPath; /*!< Path of the lPowerProbe program */
   char** m_programEnv; /*!< Environnement variables from program */

   unsigned int m_nbProcess;
   std::vector<unsigned int> m_pinning;
   
   std::vector<std::string> m_probesPath;
   
   unsigned int m_nbRepet;
   
   size_t m_iterationMemorySize; /*!< memory size for kernel usage */
   unsigned long int m_nbKernelIteration;
   
   std::string m_execName;
   std::vector<std::string> m_args;
   
   std::string m_outputFile;
   
   /**
    * Default values
    */
   static const unsigned int DEFAULT_NUMBER_PROCESS = 1;
   static const unsigned int DEFAULT_NUMBER_REPET = 5;
   static const unsigned long int DEFAULT_NUMBER_KERNELITER = 1000;
   static const std::string DEFAULT_OUTPUT_FILE;
   
   
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
    * \return the path used to run lPowerProbe
    */
   const std::string& getProgramPath()const;
   
   /**
    * \return get the environnement variables
    */
   char** getProgramEnv()const;
   
   /**
    * Sets the path to run lPowerProbe
    * \param numberProcess
    */
   void setProgramPath(const char* pPath);
   
   /**
    * Sets the environnement variables used to run lPowerProbe
    * \param pEnv
    */
   void setProgramEnv(char** pEnv);
   
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
    * \return false if the parsing of the code ID failed
    */
   bool setPinning(const std::string& pinning);
   
   /**
    * \return the probes paths
    */
   const std::vector<std::string>& getProbesPath()const;
   
   /**
    * \param probesPath a string ';' separated of paths for probes
    */
   bool setProbesPath(const std::string& probesPath);
   
   /**
    * \return the number of repetition to do
    */
   unsigned int getNbRepetition()const;
   
   /**
    * \param numberRepetition the number of repetition to run
    */
   void setNbRepetition(unsigned int numberRepetition);
   
   /**
    * \return the memory size used by one kernel iteration that the user wants for his kernel
    */
   size_t getIterationMemorySize()const;
   
   /**
    * \param iterationMemSize the memory size used by one kernel iteration the user wants for his kernel
    */
   void setIterationMemorySize(size_t iterationMemSize);
   
   /**
    * \return the number of kernel iteration
    */
   unsigned long int getNbKernelIteration()const;
   
   /**
    * \param numberKernelIteration the number of kernel iteration
    */
   void setNbKernelIteration(unsigned long int numberKernelIteration);
   
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
    * \return true if the exec is a kernel
    */
   bool isExecKernel()const;

   /**
    * \return true if we run as a daemon waiting for signals.
    */
   bool isDaemon() const;
   
   /**
    * Set the default values to unset options
    */
   void setDefaultValues();
   
   /**
    * Check if all necessary options have been set
    * \return true if there is a missing options
    */
   bool hasMissingOptions();
   
   /**
    * Specify a result file
    * \param pOutputFile
    */
   void setOutputFile(const char* pOutputFile);
   
   /**
    * Get the outputFile 
    */
   const std::string& getOutputFile()const;
   
};

#endif
