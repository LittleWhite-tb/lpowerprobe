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


/**
 * Kernel wrapper
 * 
 * This will take a kernel (compiled) and extract the function needed by the user
 * with \a loadFunction
 */
class Kernel
{
   private:
   
      void* m_pLibHandle;  /*!< Handle of the lib file */
   
   public:
      /**
       * \throws KernelException if the file can't be loaded
       * \param fileName the file to load as a kernel
       */
      Kernel(const std::string& fileName);
      
      /**
       */
      ~Kernel();
      
      /**
       * Find a function from the Kernel
       * \param fctName the function name to find
       * \return a pointer to the function or NULL if the function can't be found
       */
      void* loadFunction(const std::string& fctName);
};

#endif
