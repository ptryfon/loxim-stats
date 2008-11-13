#!/bin/bash


function singleTest() {
echo "============================================================"
echo "running test" $1 
echo "------------------------------------------------------------"

#using config prepared for memory leaks
mv ../../LoximServer/Server.conf ../../LoximServer/Server.conf.old
cp Server.conf ../../LoximServer/Server.conf

sleepTime=$7
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
echo valgrind $6 --vex-iropt-precise-memory-exns=yes --log-file=$testOutputPath $8 ../../LoximServer/Server & 
valgrind $6 --vex-iropt-precise-memory-exns=yes --log-file=$testOutputPath  $8 ../../LoximServer/Server & 
#valgrind $6 --vex-iropt-precise-memory-exns=yes ../../LoximServer/Server 2> $testOutputPath"Mixed" & 

echo "----------- SLEEPING " $sleepTime " SECS -------------------"
sleep $sleepTime
echo "----------- running sbql query -----------------------------"
../../LoximClient/Lsbql -l Administrator -P ala -m slash -f $3 &


rm -rf /tmp/loxim/* 

#ps a


#restoring old configuration
mv ../../LoximServer/Server.conf.old ../../LoximServer.conf

echo "----------- test FINISHED ----------------------------------"
echo "============================================================"
echo ""
}

function debugTest() {
echo "============================================================"
echo "running test" $1 
echo "------------------------------------------------------------"

echo $*
echo 1 $1
echo 2 $2
echo 3 $3
echo 4 $4
echo 5 $5
echo 6 $6
echo 7 $7
echo 8 $8

#using config prepared for memory leaks
mv ../../LoximServer/Server.conf ../../LoximServer/Server.conf.old
cp Server.conf ../../LoximServer/Server.conf

sleepTime=$7
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
echo valgrind $6 --vex-iropt-precise-memory-exns=yes --log-file=$testOutputPath $8 ../../LoximServer/Server & 
valgrind $6 --vex-iropt-precise-memory-exns=yes --log-file=$testOutputPath  $8 ../../LoximServer/Server & 
#valgrind $6 --vex-iropt-precise-memory-exns=yes ../../LoximServer/Server 2> $testOutputPath"Mixed" & 

echo "----------- SLEEPING " $sleepTime " SECS -------------------"
sleep $sleepTime
echo "----------- running sbql query -----------------------------"
../../LoximClient/Lsbql -l Administrator -P ala -m slash -f $3 &


rm -rf /tmp/loxim/* 

#ps a


#restoring old configuration
mv ../../LoximServer/Server.conf.old ../../LoximServer.conf

echo "----------- test FINISHED ----------------------------------"
echo "============================================================"
echo ""
}
#-------------- Tests ---------------------
#simpleTest test-name test-date test-path test-output-path test-output-type args sleepTime addtional-output-args 


function simpleTests() {
	sleepTimeT=10
	singleTest "emptyTrans" $data "tests/emptyTrans.sbql" "results" "log" "--leak-check=full --leak-resolution=med --num-callers=20" $sleepTimeT 
	singleTest "emptyTrans" $data "tests/emptyTrans.sbql" "results" "xml" "--leak-check=full --leak-resolution=med --num-callers=20" $sleepTimeT "--xml=yes" 
}
function aagg() {
	singleTest "singleCreateCompound" $data "tests/singleCreateCompound.sbql" "results" "log" "--leak-check=full --leak-resolution=med --num-callers=20" $sleepTimeT 
	singleTest "singleCreateCompound" $data "tests/singleCreateCompound.sbql" "results" "xml" "--leak-check=full --leak-resolution=med --num-callers=20" $sleepTimeT "--xml=yes" 
	singleTest "singleCreateInt" $data "tests/singleCreateInt.sbql" "results" "log" "--leak-check=full --leak-resolution=med --num-callers=20" $sleepTimeT 
	singleTest "singleCreateInt" $data "tests/singleCreateInt.sbql" "results" "xml" "--leak-check=full --leak-resolution=med --num-callers=20" $sleepTimeT "--xml=yes" 


	singleTest "singleCreateDouble" $data "tests/singleCreateDouble.sbql" "results" "log" "--leak-check=full --leak-resolution=med --num-callers=20" $sleepTimeT 
	singleTest "singleCreateDouble" $data "tests/singleCreateDouble.sbql" "results" "xml" "--leak-check=full --leak-resolution=med --num-callers=20" $sleepTimeT "--xml=yes" 

	singleTest "singleCreateString" $data "tests/singleCreateString.sbql" "results" "log" "--leak-check=full --leak-resolution=med --num-callers=20" $sleepTimeT
	singleTest "singleCreateString" $data "tests/singleCreateString.sbql" "results" "xml" "--leak-check=full --leak-resolution=med --num-callers=20" $sleepTimeT "--xml=yes" 

	singleTest "singleDerefCompound" $data "tests/singleDerefCompound.sbql" "results" "log" "--leak-check=full --leak-resolution=med --num-callers=20" $sleepTimeT 
	singleTest "singleDerefCompound" $data "tests/singleDerefCompound.sbql" "results" "xml" "--leak-check=full --leak-resolution=med --num-callers=20" $sleepTimeT "--xml=yes" 

	singleTest "singleDerefCompound2" $data "tests/singleDerefCompound2.sbql" "results" "log" "--leak-check=full --leak-resolution=med --num-callers=20" $sleepTimeT
	singleTest "singleDerefCompound2" $data "tests/singleDerefCompound2.sbql" "results" "xml" "--leak-check=full --leak-resolution=med --num-callers=20" $sleepTimeT "--xml=yes" 

	singleTest "singleDerefInt" $data "tests/singleDerefInt.sbql" "results" "log" "--leak-check=full --leak-resolution=med --num-callers=20" $sleepTimeT
	singleTest "singleDerefInt" $data "tests/singleDerefInt.sbql" "results" "xml" "--leak-check=full --leak-resolution=med --num-callers=20" $sleepTimeT "--xml=yes" 
}

function debugTest() {
	singleTest "emptyTrans" $data "tests/emptyTrans.sbql" "results" "log" "--leak-check=full --leak-resolution=med --num-callers=20" "10" "--db-attach=yes" 
}

#preparing test configuration and dirs
if [ ! -e "/tmp/loxim" ] 
then
	mkdir "/tmp/loxim"
fi


data=$(date +%Y-%m-%d-%H-%M)

echo AA
echo $#

#if [ $# -e 0 ] 
#then
#	debugTest	
	#simpleTests 
#elif [ $# -e 1 ] 
#then
#	case $1 in
#		debug) debugTest;;
#	esac
#fi

simpleTests
#debugTest

