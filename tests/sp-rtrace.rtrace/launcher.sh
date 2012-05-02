#!/bin/sh

#
# Launcher script to launch sp-rtrace and wait until target process
# terminates.
#

#
# Usage:
#  $1 application to execute
#  $2 output directory
#  $3 postprocessor options
#  $4 start option
#  $5 managed mode option
#  $6 backtrace depth
#

#echo sp-rtrace -pmemory $2 $3 $4 $5 $6 -x $1 >> launch.log
sp-rtrace -e memory $2 $3 $4 $5 $6 -x $1 &
sleep 1

app_name=${1##*/}
app_name=${app_name%% *}
while pidof $app_name > /dev/null ; do
	sleep 1
done

exit 0
