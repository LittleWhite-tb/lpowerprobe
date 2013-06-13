#include "Probe.hpp"

#include <iostream>
#include <dlfcn.h>

#include "ProbeLoadingException.hpp"

Probe::Probe(const std::string& path)
{
   this->pLibHandle = dlopen(path.c_str(),RTLD_LAZY);
   if ( this->pLibHandle == NULL )
   {
      throw ProbeLoadingException("Error to load probe : '" + path + "' (" + dlerror() + ")");
   }
   
   this->evaluationInit =(evalInit) dlsym(this->pLibHandle,"evaluationInit");
   if ( this->evaluationInit == NULL )
   {
      throw ProbeLoadingException("Error to load evaluationInit in probe : '" + path + "'");
   }
   
   this->evaluationStart =(evalGet) dlsym(this->pLibHandle,"evaluationStart");
   if ( this->evaluationStart == NULL )
   {
      throw ProbeLoadingException("Error to load evaluationStart in probe : '" + path + "'");
   }
   
   this->evaluationStop = (evalGet) dlsym(this->pLibHandle,"evaluationStop");
   if ( this->evaluationStop == NULL )
   {
      throw ProbeLoadingException("Error to load evaluationStop in probe : '" + path + "'");
   }

   this->evaluationClose = (evalClose) dlsym(this->pLibHandle,"evaluationClose");
   if ( this->evaluationClose == NULL )
   {
      throw ProbeLoadingException("Error to load evaluationClose in probe : '" + path + "'");
   }

   std::cout << "'" << path << "' successfully loaded" << std::endl;
   
   // Now, we just start the probe
   if ( this->evaluationInit )
   {
      this->pProbeHandle = this->evaluationInit();
   }
}

Probe::~Probe()
{
   this->evaluationClose(this->pProbeHandle);
   
   dlclose(this->pLibHandle); // Should return zero
   
   std::cerr << "[sampler : INFO] ProbeLib successfully closed" << std::endl;
}
   
double Probe::startMeasure()
{
   return this->evaluationStart(this->pProbeHandle);
}

double Probe::stopMeasure()
{
   return this->evaluationStop(this->pProbeHandle);
}
