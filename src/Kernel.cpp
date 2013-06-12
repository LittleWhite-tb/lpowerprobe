#include "Kernel.hpp"

#include <iostream>

#include <dlfcn.h>

Kernel::Kernel(const std::string& fileName)
{
   m_pLibHandle = dlopen(fileName.c_str(), RTLD_NOW);
   if (m_pLibHandle == NULL)
   {
      throw KernelException("Error: Library name " +  std::string(dlerror()));
   }
}

Kernel::~Kernel()
{
   dlclose(m_pLibHandle);
}
      
void* Kernel::loadFunction(const std::string& fctName)
{
   //Clear any error
   dlerror ();
   
   void* pKernelFct = dlsym (m_pLibHandle, fctName.c_str());
   if (pKernelFct == NULL)
   {
      std::cerr << "Fail to find the '" << fctName.c_str () << " ' in the kernel" << std::endl;
      return NULL;
   }
   
   return pKernelFct;
}
