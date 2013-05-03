#ifndef KERNEL_HPP
#define KERNEL_HPP

#include <string>

/**
 * @class KernelException
 * @brief Thrown when GCC failed to compile the kernel
 */
class KernelException: public std::exception
{
   public:
   
      /**
       * \param message reason of the exception
       */
      KernelException(const std::string& message)
         :message(message)
      {
      }
   
      virtual ~KernelException(void)throw() {}

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

class Kernel
{
   private:
   
      void* m_pLibHandle;
   
   public:
      Kernel(const std::string& fileName);
      ~Kernel();
      
      void* loadFunction(const std::string& fctName);
};

#endif
