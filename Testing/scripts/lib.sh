#!/bin/bash

loximKiller="./loximKiller.sh"
loximData="/tmp/loxim/*"
loximDataDir="/tmp/loxim"
loxim_server="../../src/loxim_server"
lsbql="../../src/lsbql"
mySuffix="-ft1"
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

	rm /tmp/sbclasses  /tmp/sbdefault  /tmp/sbindexdata  /tmp/sbindexmeta /tmp/sbinterfaces  /tmp/sbmap  /tmp/sbroots  /tmp/sbviews /tmp/szbd_logs 2>/dev/null

}
makeTest() {
	echo "++++++++++++ running test $1 +++++++++++++++++" 
	let ret=1
	while [ ! $ret -eq 0 ]
	do
		bash -c "$1" &
		$loximKiller $2 $loximData
		let ret=$?
	done
	rm $loximData
	cleanDefault
	sleep 1
	echo "++++++++++++ test run $1 finished +++++++++++++++"
}



