#!/bin/bash

loximKiller="./loximKiller.sh"
loximData="/tmp/loxim/*"
loximDataDir="/tmp/loxim"
loxim_server="../../src/loxim_server"
lsbql="../../src/lsbql"
mySuffix="-infoLevel"
cleanData=$(date +%Y-%m-%d)
fullData=$(date +%Y-%m-%d-%H-%M)$mySuffix
data=$cleanData$mySuffix
memcheck="valgrind"


helgrind="valgrind --tool=helgrind"

#you can add sufix to standard dirname by param
prepareDirs() {

	if [ ! -e $loximDataDir ]
	then
		mkdir $loximDataDir
	fi

}

cleanDefault() {
	
#	ipcs -s | while read a; do ipcrm -s `echo $a | cut -f2 -d" "` ; done 2>/dev/null
	rm /tmp/sbschemas /tmp/sbclasses  /tmp/sbdefault  /tmp/sbindexdata  /tmp/sbindexmeta /tmp/sbinterfaces  /tmp/sbmap  /tmp/sbroots  /tmp/sbviews /tmp/szbd_logs 2>/dev/null
	rm /tmp/loxim/* 2>/dev/null

}
makeTest() {
	echo "++++++++++++ running test $1 +++++++++++++++++" 
	ret=1
	while [  $ret -eq 1 ]
	do
		bash -c "$1" &
		$loximKiller $2 $loximData
		ret=$?
	done
	rm $loximData 2> /dev/null
	cleanDefault
	sleep 1
	echo "++++++++++++ test run $1 finished +++++++++++++++"
}



