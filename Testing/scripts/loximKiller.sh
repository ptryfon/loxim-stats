#!/bin/bash

#script have tow parameters 
#1: how many seconds to sleep before killing loxim server and clients
#2: path to clean, after killing server(data on default path are cleaned automatically)

source lib.sh


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
			let ret=1
			cleanDefault
			if [ $# -eq 2 ] 
			then
				rm $2
			fi
		fi
	done

	sleep 2

	ps -a | while read a
	do
		pid=`echo $a | grep -v grep | grep lsbql | sed "s/^[ ]*\([^ ]\)/\1/g" | cut -d" " -f1`
		if [ ${#pid} -gt 0 ] 
		then
			kill -9 $pid
			let ret=1
		fi
	done

else 
	echo "usage: ./loximKiller timeToSleepBeforeMassacre [filesToCleanAfterKilling]"
fi
exit $ret
