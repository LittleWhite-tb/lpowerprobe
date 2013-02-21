#ifndef EXPERIMENTATION_HPP
#define EXPERIMENTATION_HPP

#include <vector>
#include <string>

/**
 * Set up the experimentation context and start the benchmarking using \a Runner
 * The experimentation is following the following path :
 * - Looping for all experimentation
 * - - Looping for all process
 * - - - Creating fork
 * - - - Looping for all metarepetition (Runner)
 * - - - Start test program (Runner)
 * 
 * The forks gets an higher priority and can be pinned 
 */
class Experimentation
{
private:

   std::vector<std::string> m_probePaths; /*!< List of probes to load */

   unsigned int m_nbProcess;  /*!< Number of process to start */
   std::vector<unsigned int> m_pinning; /*!< Process pinning */
   
   unsigned int m_nbRepet; /*!< Number of repetition */
   unsigned int m_nbMetaRepet; /*!< Number of meta repetition */
   
public:
   /**
    * \param nbProcess the number of process to start
    * \param pinning the process pinning
    * \param nbRepet the number of repetition
    * \param nbMetaRepet the number of meta repetition
    */
   Experimentation(unsigned int nbProcess, const std::vector<unsigned int>& pinning, unsigned int nbRepet, unsigned int nbMetaRepet);
   
   /**
    */
   ~Experimentation();
   
   /**
    * Starts the experimentation process
    * \param test the program to bench
    * \param args the arguments to pass to the program
    */
   void startExperimentation(const std::string& test, const std::vector<std::string>& args);
};

#endif
