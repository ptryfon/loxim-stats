#!/bin/bash


function simgleTest() {
echo "============================================================"
echo "running test" $1 
echo "------------------------------------------------------------"

sleepTime=10
#preparing tests names 
testOutputName=$1$2"."$5
testOutputPath=$4"/"$1"/"$5"/"$testOutputName
outputPath=$4"/"$1"/"$5
if [ ! -e "$outputPath" ]
then
mkdir $outputPath
mkdir $outputPath"/log"
mkdir $outputPath"/xml"
fi
#cleaning directories
rm -rf /tmp/loxim/*
#mkdir /tmp/loxim

#m problemyZWychodzeniem2

echo "----------- starting Loxim Server --------------------"
valgrind $6 --vex-iropt-precise-memory-exns=yes --log-file=$testOutputPath ../../LoximServer/Server & 
#valgrind $6 --vex-iropt-precise-memory-exns=yes ../../LoximServer/Server 2> $testOutputPath"Mixed" & 

echo "----------- SLEEPING " $sleepTime " SECS -------------------"
sleep $sleepTime
echo "----------- running sbql query -----------------------------"
../../LoximClient/Lsbql -l Administrator -P ala -m slash -f $3 
rm -rf /tmp/loxim/* 
#ps a
echo "----------- test FINISHED ----------------------------------"
echo "============================================================"
echo ""
}

#-------------- Tests ---------------------
#simpleTest test-name test-date test-path test-output-path test-output-type args addtional-output-args 

function simpleTests() {
	simgleTest "emptyTrans" $data "tests/emptyTrans.sbql" "results" "log" "--leak-check=full --leak-resolution=med --num-callers=20" 
	simgleTest "emptyTrans" $data "tests/emptyTrans.sbql" "results" "xml" "--leak-check=full --leak-resolution=med --num-callers=20 --xml=yes" 

	simgleTest "singleCreateCompound" $data "tests/singleCreateCompound.sbql" "results" "log" "--leak-check=full --leak-resolution=med --num-callers=20" 
	simgleTest "singleCreateCompound" $data "tests/singleCreateCompound.sbql" "results" "xml" "--leak-check=full --leak-resolution=med --num-callers=20 --xml=yes" 
	simgleTest "singleCreateInt" $data "tests/singleCreateInt.sbql" "results" "log" "--leak-check=full --leak-resolution=med --num-callers=20" 
	simgleTest "singleCreateInt" $data "tests/singleCreateInt.sbql" "results" "xml" "--leak-check=full --leak-resolution=med --num-callers=20 --xml=yes" 


	simgleTest "singleCreateDouble" $data "tests/singleCreateDouble.sbql" "results" "log" "--leak-check=full --leak-resolution=med --num-callers=20" 
	simgleTest "singleCreateDouble" $data "tests/singleCreateDouble.sbql" "results" "xml" "--leak-check=full --leak-resolution=med --num-callers=20 --xml=yes" 

	simgleTest "singleCreateString" $data "tests/singleCreateString.sbql" "results" "log" "--leak-check=full --leak-resolution=med --num-callers=20" 
	simgleTest "singleCreateString" $data "tests/singleCreateString.sbql" "results" "xml" "--leak-check=full --leak-resolution=med --num-callers=20 --xml=yes" 

	simgleTest "singleDerefCompound" $data "tests/singleDerefCompound.sbql" "results" "log" "--leak-check=full --leak-resolution=med --num-callers=20" 
	simgleTest "singleDerefCompound" $data "tests/singleDerefCompound.sbql" "results" "xml" "--leak-check=full --leak-resolution=med --num-callers=20 --xml=yes" 

	simgleTest "singleDerefCompound2" $data "tests/singleDerefCompound2.sbql" "results" "log" "--leak-check=full --leak-resolution=med --num-callers=20" 
	simgleTest "singleDerefCompound2" $data "tests/singleDerefCompound2.sbql" "results" "xml" "--leak-check=full --leak-resolution=med --num-callers=20 --xml=yes" 

	simgleTest "singleDerefInt" $data "tests/singleDerefInt.sbql" "results" "log" "--leak-check=full --leak-resolution=med --num-callers=20" 
	simgleTest "singleDerefInt" $data "tests/singleDerefInt.sbql" "results" "xml" "--leak-check=full --leak-resolution=med --num-callers=20 --xml=yes" 
}

function debugTest() {
	simgleTest "emptyTrans" $data "tests/emptyTrans.sbql" "results" "log" "--leak-check=full --leak-resolution=med --num-callers=20 --db-attach=yes" 
}

#preparing test configuration and dirs
if [ ! -e "/tmp/loxim" ] 
then
mkdir "/tmp/loxim"
fi

mv ../../LoximServer/Server.conf ../../LoximServer/Server.conf.old
cp Server.conf ../../LoximServer/Server.conf
data=$(date +%Y-%m-%d-%H-%M)

if [ $# -e 0 ] 
then
	simpleTests
elif [ $# -e 1 ] 
then
	case $1 in
		debug) debugTest;;
	esac
fi

#simpleTests
debugTest

#restoring old configuration
mv ../../LoximServer/Server.conf.old ../../LoximServer.conf
