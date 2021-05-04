#!/bin/bash
#set -x

__pwd__=`pwd`
rootdir=`dirname $0`
rootdir=`(cd $rootdir; pwd)`

if [ ! -d $rootdir ]; then
    exit
fi

SECONDS=0
LOG_FILE=$rootdir/`basename $0`.log
# Save stdout, stderr
exec 3>&1 4>&2
# Redirect output ( > ) into a named pipe ( >() ) running "tee"
exec >  >(tee -i ${LOG_FILE}) 2>&1

function finish {
    EXIT_CODE=$?
    pkill -P $$
    SCRIPT_RUN_TIME=$SECONDS
    # Restore output
    exec 1>&3 2>&4
    exit $EXIT_CODE
}
trap finish EXIT

