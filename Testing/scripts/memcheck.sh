#!/bin/bash


source lib.sh

output="../results/memoryLeaks/"
outputDir=$output$cleanData
fileName=$2"-"$fullData".txt"
standardOpt="--leak-check=full --leak-resolution=high --num-callers=30 --vex-iropt-precise-memory-exns=yes"
if [ ! -e $outputDir ]
then
	mkdir $outputDir
fi

logfile=$outputDir"/"$fileName

if [ $# -ge 2 ] 
then
	echo "++++++++++++++ starting memcheck test for $1 +++++++++++++"
	echo "====================== starting loxim ======================"
	if [ $# -eq 3 ]
	then
		$memcheck $standardOpt --log-file=$logfile $3 $loxim_server &
	else 
		$memcheck $standardOpt --log-file=$logfile $loxim_server & 
	fi
	sleep 15
	#running script 
	echo "===================== starting script ======================"
	$1
	sleep 5
	echo "================== shuting down loxim ======================"	
	$lsbql -l root -P aa -m slash -f ../tests/simpleTests/shutdown.sbql &
	wait


	echo "+++++++++++++++++ memcheck test for $1 finished  ++++++++++++++"
else
	echo "usage memcheck test fileName [optionalArgsForMemcheck]"
fi
