#!/bin/bash

#script have 2 mandatory aruments
#*test
#*outputPrefix
#it also has one optional parameter
#*addtional helgrind options

source lib.sh

output="../results/threadsProblems/"
fileName=$2"-"$fullData
logfile=$output$fileName

if [ $# -ge 2 ] 
then
	echo "++++++++++++++ starting helgrind test for $1 +++++++++++++"
	echo "====================== starting loxim ======================"
	if [ $# -eq 3 ]
	then
		$helgrind --log-file=$logfile $3 $loxim_server &
	else 
		$helgrind --log-file=$logfile $loxim_server & 
	fi
	sleep 15
	#running script 
	echo "===================== starting script ======================"
	$1
	sleep 5
	echo "================== shuting down loxim ======================"	
	$lsbql -l root -P aa -m slash -f ../tests/simpleTests/shutdown.sbql &
	wait


	echo "+++++++++++++++++ helgrind test for $1 finished  ++++++++++++++"
else
	echo "usage ./helgrind test fileName [optionalArgsForHelgrind]"
fi
