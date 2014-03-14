#!/bin/bash

# load configuration
if [ $# -lt 1 ]; then
   echo "No configuration file provided to LPP killer"
   exit 1
elif ! [ -f "$1" ]; then
   echo "Invalid configuration file provided to LPP killer"
   exit 1
fi

source $1
shift

LPPPID=$(cat ${LPPPIDFILE})

# kill the LPP daemon
if [ -n "${LPPPID}" ]; then
   mkdir -p ${RES_DIR}
   kill -SIGTERM ${LPPPID}

   # can't wait for the pid here as it is not our son
   while [ -e "/proc/${LPPPID}" ]; do
      sleep 1
   done

   # copy the local output to a shared location
   mv ${LPPOUTPUT} ${RES_DIR}/lppout_$(hostname -s)_$(date +"%d%m%y%H%M%S").csv
fi
