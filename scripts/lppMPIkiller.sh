#!/bin/bash

LPPPID=$(cat /tmp/lppDaemonPID)

if [ -n "${LPPPID}" ]; then
   kill -SIGTERM ${LPPPID}
   wait ${LPPPID}
fi
