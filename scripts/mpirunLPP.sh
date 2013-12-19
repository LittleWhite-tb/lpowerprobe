#!/bin/bash

# check we have a config as a first argument
if [ $# -lt 1 ]; then
   echo "You must at least specify a configuration file"
   exit 1
elif ! [ -f $1 ]; then
   echo "The first argument must be a valid configuration file"
   exit 1
fi

LPPCONF=$1
source ${LPPCONF}
shift

# build "light" arguments with only relevant MPI options for running lpp
largs=""
appendArg=false
ignArg=false
ignopts="-c -n --n -np" 
opts="-c -n --n -np -hostfile --hostfile -machinefile --machinefile -H -host --host"
for arg in "$@"; do
   if ${ignArg}; then
      ignArg=false
      continue
   fi

   if ${appendArg}; then
      appendArg=false
      largs="${largs} ${arg}"
      continue
   fi

   for opt in ${ignopts}; do
      if [ "${arg}" == "${opt}" ]; then
         ignArg=true
         break
      fi
   done

   if ${ignArg}; then
      continue
   fi

   for opt in ${opts}; do
      if [ "${arg}" == "${opt}" ]; then
         appendArg=true
         largs="${largs} ${arg}"
         break
      fi
   done
done

# arguments to run the program
pargs=""
appendArg=false
execFound=false
for arg in "$@"; do
   if ${appendArg}; then
      if ! ${execFound}; then
         appendArg=false
      fi
      pargs="${pargs} ${arg}"
      continue
   fi

   for opt in ${opts}; do
      if [ "${arg}" == "${opt}" ]; then
         appendArg=true
         pargs="${pargs} ${arg}"
         break
      fi
   done

   # we already appened the argument?
   if ${appendArg}; then
      continue;
   fi

   # not a known argument but starts with -
   if [[ ${arg} == -* ]]; then
      echo "Unsupported MPI argument: ${arg}"
      exit 1
   fi

   # probably the executable name here
   pargs="${pargs} ${LPPLOADER} ${LPPCONF} ${arg}"
   execFound=true
   appendArg=true
done

# run lpp as a deamon on all nodes
mpirun -npernode 1 ${largs} ${LPP} ${LPPOPTS}

for r in $(seq ${NREPET}); do
   mpirun ${pargs}
done

# now kill the LPP daemon
mpirun -npernode 1 ${largs} ${LPPKILLER} ${LPPCONF}
