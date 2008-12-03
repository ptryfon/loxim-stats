#!/bin/bash

#if you have any questions/found any bugs mailto: gtimoszuk@gmail.com

#you need loxim and google perfTools to run this script

#you can assume that:
#*the loxim_server is going to be started automatically
#*the loxim server is going to been shutdown cleanly 

#you have to remember:
#*testing script have to finish to get result 

#you are going to receive information about:
#*how much memory is allocated
#*how much memory is used
#*how many objects are allocated
#*how many objects are used

loxim_server="../../src/loxim_server"
lsbql="../../src/lsbql"
prefix="../results/heap/"

if [ $# -eq 2 ] 
then
	if [ -e $2.0001.heap ] 
	then
		rm $2.0001.heap
	fi
	#echo $@
	echo "====================== starting loxim ======================"
	HEAPPROFILE=$2 $loxim_server &
	sleep 5
	#running script 
	echo "===================== starting script ======================"
	$1
	sleep 5
	echo "================== shuting down loxim ======================"	
	$lsbql -l root -P aa -m slash -f ../tests/simpleTests/shutdown.sbql &
	wait

	pprof --alloc_space --ps $loxim_server $2.0001.heap > $prefix$2AllocSpace.ps
	pprof --alloc_space --text $loxim_server $2.0001.heap > $prefix$2AllocSpace.text
	pprof --alloc_space --gif $loxim_server $2.0001.heap > $prefix$2AllocSpace.gif
	
	pprof --alloc_objects --ps $loxim_server $2.0001.heap > $prefix$2AllocObjects.ps
	pprof --alloc_objects --text $loxim_server $2.0001.heap > $prefix$2AllocObjects.text
	pprof --alloc_objects --gif $loxim_server $2.0001.heap > $prefix$2AllocObjects.gif

	pprof --inuse_space --ps $loxim_server $2.0001.heap > $prefix$2InUseSpace.ps
	pprof --inuse_space --text $loxim_server $2.0001.heap > $prefix$2InUseSpace.text
	pprof --inuse_space --gif $loxim_server $2.0001.heap > $prefix$2InUseSpace.gif
	
	pprof --inuse_objects --ps $loxim_server $2.0001.heap > $prefix$2InUseObjects.ps
	pprof --inuse_objects --text $loxim_server $2.0001.heap > $prefix$2InUseObjects.text
	pprof --inuse_objects --gif $loxim_server $2.0001.heap > $prefix$2InUseObjects.gif
	
	mv $2.0001.heap $prefix$2.0001.heap
		
else 
	echo "usage: perftools [sciptToRun] [outputBeginnig]"
fi
