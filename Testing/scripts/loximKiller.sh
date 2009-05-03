#!/bin/bash

#script have tow parameters 
#1: how many seconds to sleep before killing loxim server and clients

source "$LOXIM_HOME/Testing/scripts/library.sh"

let ret=0
if [ $# -ge 1 ] 
then
	sleep $1
	ps a | while read a
	do
		pid=`echo $a | grep -v grep | grep loxim_server | sed "s/^[ ]*\([^ ]\)/\1/g" | cut -d" " -f1`
		if [ ${#pid} -gt 0 ] 
		then
			kill -9 $pid
			findLoximConfig
			findLoximStoreObjects
			removeLoximStoreObjects
			ret=1
		fi
	done

	sleep 2

	ps a | while read a
	do
		pid=`echo $a | grep -v grep | grep lsbql | sed "s/^[ ]*\([^ ]\)/\1/g" | cut -d" " -f1`
		if [ ${#pid} -gt 0 ] 
		then
			kill -9 $pid
			ret=1
		fi
	done

else 
	echo "usage: ./loximKiller timeToSleepBeforeMassacre"
fi
exit $ret
