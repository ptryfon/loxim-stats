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

source lib.sh
prepareDirs gtT

echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
echo "+++++++++++++++++ google perf tools test begin ++++++++++++"
echo "---------- testing script $1 output sufix $2 --------"

prefix="../results/heap/"
prefix=$prefix"perfTools"$2$fullData

if [ ! -e $prefix ]
then
	mkdir $prefix
fi
prefix=$prefix"/"

if [ $# -eq 2 ] 
then
	if [ -e $2.????.heap ] 
	then
		rm $2.????.heap 2> /dev/null
	fi
	#echo $@
	echo "====================== starting loxim ======================"
	( export HEAP_PROFILE_ALLOCATION_INTERVAL=90000000000 ; export HEAP_PROFILE_INUSE_INTERVAL=90000000000 ; HEAPPROFILE=$2 $loxim_server ) &
	sleep 5 
	#running script 
	echo "===================== starting script ======================"
	$1
	sleep 5
	echo "================== shuting down loxim ======================"	
	$lsbql -l root -P aa -m slash -f ../tests/simpleTests/shutdown.sbql &
	wait
	
	pprof --alloc_space --ps $loxim_server $2.0001.heap > $prefix$2AllocSpace.ps
	pprof --alloc_space --pdf $loxim_server $2.0001.heap > $prefix$2AllocSpace.pdf
	pprof --alloc_space --text $loxim_server $2.0001.heap > $prefix$2AllocSpace.text
	pprof --alloc_space --gif $loxim_server $2.0001.heap > $prefix$2AllocSpace.gif
	
	pprof --alloc_objects --ps $loxim_server $2.0001.heap > $prefix$2AllocObjects.ps
	pprof --alloc_objects --pdf $loxim_server $2.0001.heap > $prefix$2AllocObjects.pdf
	pprof --alloc_objects --text $loxim_server $2.0001.heap > $prefix$2AllocObjects.text
	pprof --alloc_objects --gif $loxim_server $2.0001.heap > $prefix$2AllocObjects.gif

	pprof --inuse_space --ps $loxim_server $2.0001.heap > $prefix$2InUseSpace.ps
	pprof --inuse_space --pdf $loxim_server $2.0001.heap > $prefix$2InUseSpace.pdf
	pprof --inuse_space --text $loxim_server $2.0001.heap > $prefix$2InUseSpace.text
	pprof --inuse_space --gif $loxim_server $2.0001.heap > $prefix$2InUseSpace.gif
	
	pprof --inuse_objects --ps $loxim_server $2.0001.heap > $prefix$2InUseObjects.ps
	pprof --inuse_objects --pdf $loxim_server $2.0001.heap > $prefix$2InUseObjects.pdf
	pprof --inuse_objects --text $loxim_server $2.0001.heap > $prefix$2InUseObjects.text
	pprof --inuse_objects --gif $loxim_server $2.0001.heap > $prefix$2InUseObjects.gif
	
		
else 
	echo "usage: perftools [sciptToRun] [outputBeginnig]"
fi

rm *.heap 2> /dev/null
echo "+++++++++++++++++ google perf tools test end ++++++++++++++"
