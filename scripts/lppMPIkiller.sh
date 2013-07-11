#!/bin/bash

LPPPID=$(cat /tmp/lppDaemonPID)
LPPOUTPUT="/tmp/output.csv"
RES_DIR=${HOME}/nfs

if [ -n "${LPPPID}" ]; then
   kill -SIGTERM ${LPPPID}

   # can't wait for the pid here as it is not our son
   while [ -e "/proc/${LPPPID}" ]; do
      sleep 1
   done

   # copy the local output to a shared location
   cp ${LPPOUTPUT} ${RES_DIR}/lppout_$(hostname -s)_$(date +"%d%m%y%H%M%S").csv
fi
