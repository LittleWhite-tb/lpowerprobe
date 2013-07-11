#!/bin/bash

LPPPID=$(cat /tmp/lppDaemonPID)
LPPOUTPUT="/tmp/output.csv"
RESDIR=${HOME}

if [ -n "${LPPPID}" ]; then
   kill -SIGTERM ${LPPPID}

   # can't wait for the pid here as it is not our son
   while [ -e "/proc/${LPPPID}" ]; do
      sleep 1
   done

   # copy the local output to a shared location
   cp ${LPPOUTPUT} ${HOME}/lppout_$(hostname -s).csv
fi
