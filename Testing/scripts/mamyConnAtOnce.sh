#!/bin/bash

#script have one paramater number of clients to create
#script will kill all lsbql clients at the end so be cereful

loxim_server="../../src/loxim_server"
lsbql="../../src/lsbql"
let x=0

if [ $# -eq 1 ] 
then

	while [ $x -lt $1 ]
	do
		((x++))
		$lsbql -l root -P aa -m slash -f ../tests/simpleTests/ &
	done

	sleep 5 

	ps -a | while read a
		do
		pid=`echo $a | grep -v grep | grep lsbql | sed "s/^[ ]*\([^ ]\)/\1/g" | cut -d" " -f1`
		if [ ${#pid} -gt 0 ] 
		then
			kill -9 $pid
		fi
	done
else 
	echo "usage manyAtOnce [numberOfSessions]"
fi

