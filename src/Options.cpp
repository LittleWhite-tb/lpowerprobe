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

#include "Options.hpp"

#include <iostream>
#include <sstream>
#include <cassert>

const std::string Options::DEFAULT_OUTPUT_FILE="output.csv";

void Options::checkOverride(unsigned int oldValue, unsigned int newValue, const std::string& optionName)
{
   if ( oldValue != 0 && oldValue != newValue )
   {
      std::cerr << "You are overriding the value of " << optionName << std::endl;
   }
}

Options::Options()
   :m_nbProcess(0),m_nbRepet(0),m_iterationMemorySize(0),m_nbKernelIteration(0)
{
}

Options::~Options()
{
}

const std::string& Options::getProgramPath()const
{
   return m_programPath;
}
   
void Options::setProgramPath(const char* pPath)
{
   assert(pPath);
   
   if ( !m_outputFile.empty() )
   {
      std::cerr << "You are overriding program path" << std::endl;
   }
   
   m_programPath = std::string(pPath);
   
   // Split
   size_t found = m_programPath.find_last_of("/\\");
   m_programPath = m_programPath.substr(0,found);
   m_programPath = m_programPath + "/";   // Add '/' since it is remove by the substr.
}

char** Options::getProgramEnv()const
{
   return m_programEnv;
}

void Options::setProgramEnv(char** pEnv)
{
    m_programEnv = pEnv;
}

unsigned int Options::getNbProcess()const
{
   return m_nbProcess;
}

void Options::setNbProcess(unsigned int numberProcess)
{
   checkOverride(m_nbProcess,numberProcess,"process number");
   
   m_nbProcess = numberProcess;
}

const std::vector<unsigned int>& Options::getPinning()const
{
   return m_pinning;
}

bool Options::setPinning(const std::string& pinning)
{
   std::string tmpPinning(pinning);
   size_t index=0;
   while((index = tmpPinning.find(';',index)) != std::string::npos )
   {
      tmpPinning[index]=' ';
   }
   
   std::istringstream iss(tmpPinning);
   unsigned int cpuID;
   while ( iss >> cpuID )
   {
      m_pinning.push_back(cpuID);
   }
   
   if ( !iss.eof() )
   {
      return false;
   }
   
   return true;
}

const std::vector<std::string>& Options::getProbesPath()const
{
   return m_probesPath;
}

bool Options::setProbesPath(const std::string& pinning)
{
   std::string tmpPinning(pinning);
   size_t index=0;
   while((index = tmpPinning.find(';',index)) != std::string::npos )
   {
      tmpPinning[index]=' ';
   }
   
   std::istringstream iss(tmpPinning);
   std::string path;
   while ( iss >> path )
   {
      m_probesPath.push_back(path);
   }
   
   if ( !iss.eof() )
   {
      return false;
   }
   
   return true;
}
   
unsigned int Options::getNbRepetition()const
{
   return m_nbRepet;
}

void Options::setNbRepetition(unsigned int numberRepetition)
{
   checkOverride(m_nbRepet,numberRepetition,"repetitions number");
   
   m_nbRepet = numberRepetition;
}

size_t Options::getIterationMemorySize()const
{
   return m_iterationMemorySize;
}

void Options::setIterationMemorySize(size_t iterationMemSize)
{
   checkOverride(m_iterationMemorySize,iterationMemSize,"iteration memory size");
   
   m_iterationMemorySize = iterationMemSize;
}

unsigned long int Options::getNbKernelIteration()const
{
   return m_nbKernelIteration;
}

void Options::setNbKernelIteration(unsigned long int numberKernelIteration)
{
   checkOverride(m_nbKernelIteration,numberKernelIteration,"kernel iteration");
   
   m_nbKernelIteration = numberKernelIteration;
}

const std::string& Options::getExecName()const
{
   return m_execName;
}

void Options::setExecName(const char* pExecName)
{
   assert(pExecName);
   
   if ( !m_execName.empty() )
   {
      std::cerr << "You are overriding exec name" << std::endl;
   }

   // Special parsing when the exec has a space
   std::istringstream iss(pExecName);
   std::string token;
   int nbToken = 0;
   while ( iss >> token )
   {
      if ( nbToken == 0 )
      {
         m_execName = token;
      }
      else
      {
         m_args.push_back(token);
      }
      nbToken++;
   }
}
   
const std::vector<std::string>& Options::getArgs()const
{
   return m_args;
}

void Options::addArg(const char* pArgs)
{
   assert(pArgs);
   
   m_args.push_back(std::string(pArgs));
}

bool Options::isDaemon() const
{
   return m_execName.empty();
}

bool Options::isExecKernel()const
{
   if (isDaemon()) 
   {
      return false;
   }

   // Actually the check is only based on having a .s extension
   if ( m_execName[m_execName.length()-2] == '.' &&
        m_execName[m_execName.length()-1] == 's' )
   {
      return true;
   }
         
   return false;
}

void Options::setDefaultValues()
{
   if ( m_nbProcess == 0 )
   {
      std::cout << "Number of process set to default value -> " << DEFAULT_NUMBER_PROCESS << std::endl;
      m_nbProcess = DEFAULT_NUMBER_PROCESS;
   }
   
   if ( m_nbRepet == 0 )
   {
      std::cout << "Number of repetition set to default value -> " << DEFAULT_NUMBER_REPET << std::endl;
      m_nbRepet = DEFAULT_NUMBER_REPET;
   }

   if ( isExecKernel() && m_iterationMemorySize == 0 )
   {
      std::cout << "Number of kernel iteration set to default value -> " << DEFAULT_NUMBER_KERNELITER << std::endl;
      m_nbKernelIteration = DEFAULT_NUMBER_KERNELITER;
   }
   
   if ( m_outputFile.empty() )
   {
      std::cout << "Output file set to default value -> " << DEFAULT_OUTPUT_FILE << std::endl;
      m_outputFile = DEFAULT_OUTPUT_FILE;
   }
}

bool Options::hasMissingOptions()
{  
   // Additional check to tell that some options are ignored
   // TO FIX/TODO : Maybe this test should not be here
   if (isDaemon()) {
      if (m_iterationMemorySize) {
         std::cout << "Iteration memory size ignored in daemon mode" << std::endl;
         m_iterationMemorySize = 0;
      }

      if (m_nbKernelIteration) {
         std::cout << "Iteration count ignored in daemon mode" << std::endl;
         m_nbKernelIteration = 0;
      }

      if (m_nbProcess != 1) {
         std::cout << "Duplication argument ignored in daemon mode" << std::endl;
         m_nbProcess = 1;
      }

      if (m_nbRepet > 1) {
         std::cout << "Nothing to repeat in daemon mode, ignoring the argument." << std::endl;
         m_nbRepet = 1;
      }

   } else if (!isExecKernel()) {
      if ( m_iterationMemorySize )
      {
         std::cout << "The iteration memory size that you have specified is ignored since we are running a program" << std::endl;
         m_iterationMemorySize = 0;
      }

      if ( m_nbKernelIteration )
      {
         std::cout << "The number of kernel iteration that you have specified is ignored since we are running a program" << std::endl;
         m_nbKernelIteration = 0;
      }
   }
   
   if ( m_nbProcess != 1 )
   {
      if ( m_pinning.size() == 0 || m_pinning.size() != m_nbProcess )
      {
         std::cout << "You did not pin'd all the process reqd" << std::endl;
         return true;
      }
   }

   return false;
}

void Options::setOutputFile(const char* pOutputFile)
{
   assert(pOutputFile);
   
   if ( !m_outputFile.empty() )
   {
      std::cerr << "You are overriding exec name" << std::endl;
   }
   
   m_outputFile = std::string(pOutputFile);
}

const std::string& Options::getOutputFile()const
{
   return m_outputFile;
}
