#!/bin/bash

server=../LoximServer/Server
client=../LoximClient/Lsqbl

#function has one paremeter testPath
function hasShutdown() {
if [ `cat $1 | grep "#shutdown" | wc -l` -eq 0 ]
then
	cat $1; echo "#shutdown"; echo "/"
else
	cat $1
fi	
}

function printUsage() {
echo "usage test ..."

}

#function has one argument test type [simple|advanced]+     and runs all kinds of tests in defined test type
function runAll() {

echo "usage test ..."

}

#function has two parameters [simple|advanced]+ [memoryLeaks|correctness|threadsProblems]+    and runs tests of defined kind that are defined in test type
function runOneKind() {

echo "usage test ..."
}

 
#function has three parameters [simple|advanced]+ [memoryLeaks|correctness|threadsProblems]+ testPath+  and runs defined test
function runOneTest() {

echo "usage test ..."
}

case $# in
	0) printUsage;;
	1) runAll $1;;
	2) runOneKind $1 $2;;
	3) runOne $1 $2 $3;;
esac

hasShutdown $1
