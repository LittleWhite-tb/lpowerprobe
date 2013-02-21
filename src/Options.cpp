#include "Options.hpp"

#include <iostream>
#include <sstream>
#include <cassert>

void Options::checkOverride(unsigned int oldValue, unsigned int newValue, const std::string& optionName)
{
   if ( oldValue != 0 && oldValue != newValue )
   {
      std::cerr << "You are overriding the value of " << optionName << std::endl;
   }
}

Options::Options()
   :m_nbProcess(0),m_nbRepet(0),m_nbMetaRepet(0)
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
   
   m_execName = std::string(pExecName);
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
}

bool Options::hasMissingOptions()
{
   if ( m_execName.empty() )
   {
      return true;
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
