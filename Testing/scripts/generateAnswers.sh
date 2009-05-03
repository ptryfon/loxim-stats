#!/bin/bash

source "$LOXIM_HOME/Testing/scripts/library.sh"

findLoximConfig
findLoximStoreObjects

#simple
cat $LOXIM_HOME"/Testing/tests/simpleTests/allH" | while read LINE; do
	resultname=`echo $LINE | cut -f1 -d" " | cut -f1 -d"."`".txt"
	mode=`echo $LINE | cut -f2 -d" "`
	testname=`echo $LINE | cut -f1 -d" "`
	runLoximServer
	$loximKiller 50 &
	sleep 10
	$lsbql -l root -P a -m $mode -f $LOXIM_HOME/Testing/tests/simpleTests/$testname > $LOXIM_HOME/Testing/answers/simple/$resultname 
	stopLoximServer
	removeLoximStoreObjects
	wait
done 

#advanced
cat $LOXIM_HOME"/Testing/tests/advancedTests/all.txt" | while read LINE; do
	runLoximServer
	$loximKiller 50 &
	sleep 10
	echo $LINE
	resultname=`echo $LINE | cut -f1 -d" " | cut -f1 -d"."`".txt"
	echo $resultname
	rm $LOXIM_HOME/Testing/answers/advanced/$resultname
	cat $LOXIM_HOME"/Testing/tests/advancedTests/"$LINE | while read TESTNAMES; do
		mode=`echo $TESTNAMES | cut -f2 -d" "`
		echo mode $mode
		testname=`echo $TESTNAMES | cut -f1 -d" "`
		echo testname $testname
		$lsbql -l root -P a -m $mode -f $LOXIM_HOME/Testing/tests/simpleTests/$testname >> $LOXIM_HOME/Testing/answers/advanced/$resultname
	done
	stopLoximServer
	removeLoximStoreObjects
	wait
done


