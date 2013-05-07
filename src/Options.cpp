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
   :m_nbProcess(0),m_nbRepet(0),m_nbMetaRepet(0),m_iterationMemorySize(0),m_nbKernelIteration(0)
{
}

Options::~Options()
{
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
   
unsigned int Options::getNbMetaRepetition()const
{
   return m_nbMetaRepet;
}

void Options::setNbMetaRepetition(unsigned int numberMetaRepetition)
{
   checkOverride(m_nbMetaRepet,numberMetaRepetition,"repetitions number");
   
   m_nbMetaRepet = numberMetaRepetition;
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

bool Options::isExecKernel()const
{
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
   
   if ( m_nbMetaRepet == 0 )
   {
      std::cout << "Number of meta repetition set to default value -> " << DEFAULT_NUMBER_METAREPET << std::endl;
      m_nbMetaRepet = DEFAULT_NUMBER_METAREPET;
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
   if ( m_execName.empty() )
   {
      std::cout << "You did not give an program to run" << std::endl;
      return true;
   }
   
   // Additional check to tell that some options are ignored
   // TO FIX/TODO : Maybe this test should not be here
   if ( !isExecKernel() && m_iterationMemorySize )
   {
      std::cout << "The iteration memory size that you have specified is ignored since we are running a program" << std::endl;
   }
   if ( !isExecKernel() && m_nbKernelIteration )
   {
      std::cout << "The number of kernel iteration that you have specified is ignored since we are running a program" << std::endl;
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
