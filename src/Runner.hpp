#ifndef RUNNER_HPP
#define RUNNER_HPP

#include <vector>
#include <string>
#include <fstream>

#include <semaphore.h>

#include "Probe.hpp"

/**
 * Handler to run and benchmark a test
 * The class load all the probes passed to the constructor. Before starting
 * the real test with \a start(), a overhead benchmarking is done to estimate the libraries
 * costs. Finally, the real test is launch N metarepetition and the results
 * are outputted in a file. The results gets the overhead removed before getting
 * outputted.
 */
class Runner
{
private:
   typedef std::vector<Probe*> ProbeList; 
   std::ofstream m_resultFile;   /*!< */

   ProbeList m_probes;  /*!< Probe to use during a test */
   
   
   unsigned int m_nbMetaRepet;/*!< Number of repetition to do */
   unsigned int m_nbProcess;  /*!< Number of process started */
   // Contains results for 
   // - All process
   // - All meta repease
   // - All probes
   typedef std::vector < std::vector<std::vector<std::pair<double, double> > > > GlobalResultsArray;
   GlobalResultsArray m_overheadResults;  /*!< Probe results for the overhead test */
   GlobalResultsArray m_runResults; /*!< Probe results for the run test */
   
   sem_t* m_fatherLock;    /*!< Process test synchronisation */
   sem_t* m_processLock;   /*!< Process test synchronisation */
   
   /**
    * Starter for the overhead benchmark
    * Calls evaluation with a specific test 'empty'
    * \param metaRepet actual repetition number
    * \param processNumber actual process id running this function
    */
   void calculateOverhead(unsigned int metaRepet, unsigned int processNumber);
   
   /**
    * Benchmark a test by start the measurements from the probes, running the test, and saving the probes results
    * \param resultArray the memory array to keep the results
    * \param test the program to evaluate
    * \param args the args to pass to the program to evaluate
    * \param metaRepet the actual repetition number
    * \param processNumber the actual process number
    */
   void evaluation(GlobalResultsArray& resultArray, const std::string& test, const std::vector<std::string>& args, unsigned int metaRepet, unsigned int processNumber);
   
   /**
    * Start the real test in a fork. Synchonises all active process starting the test
    * \param programName the program to evaluate
    * \param pArgv the args to pass to the program to evaluate
    * \param processNumber the actual process number
    */
   void startTest(const std::string& programName, char** pArgv, unsigned int processNumber);
   
   /**
    * Saves the result in a file after applying the overhead bias
    */
   void saveResults();

public:
   /**
    * Prepare a benchmark run
    * Loads the probes, allocates results table memory
    * \param probePaths list of probes to load
    * \param resultFileName file where to output the results
    * \param nbProcess the number of process that will be started
    * \param nbMetaRepet the number of meta repetition to run
    */
   Runner(const std::vector<std::string>& probePaths, const std::string& resultFileName, unsigned int nbProcess, unsigned int nbMetaRepet);
   
   /**
    */
   ~Runner();

   /**
    * Start the benchmark
    * \param test the program to bench
    * \param args the args to pass to the program to bench
    * \param processNumber the process number
    */
   void start(const std::string& test, const std::vector<std::string>& args, unsigned int processNumber);
};

#endif
