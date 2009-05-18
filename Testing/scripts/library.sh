#!/bin/bash

#global variables

loximKiller=$LOXIM_HOME"/Testing/scripts/loximKiller.sh"
loximServer=$LOXIM_HOME"/src/Programs/loxim_server"
lsbql=$LOXIM_HOME"/src/Programs/lsbql"
fullData=$(date +%Y-%m-%d-%H-%M)
memcheck="valgrind --tool=memcheck --leak-check=full --show-reachable=yes --leak-resolution=high --num-callers=30 --vex-iropt-precise-memory-exns=yes "
helgrind="valgrind --tool=helgrind"
massif="valgrind --tool=massif --max-snapshots=1000"
drd="valgrind --tool=drd"
nodefraction=0.0005
edgefraction=0.00001

declare -a loximStoreFiles

#generation of outputs names for result and output (lsbql client output)
#this functions has one parameter:
#	*test name
makeOuptunName() {
	outputCurrPath=$outputPath"/"$runNumber"_"$testType"_"$1"_"$fullData
	echo "######## output path $outputCurrPath ###########################################" 
}

makeResultName() {
	resultCurrPath=$resultPath"/"$runNumber"_"$testType"_"$1"_"$fullData
	echo "######## results path $resultCurrPath ##########################################" 
}


#searches for loxim confing in default locations
findLoximConfig() {
	if [ -e ~/.loxim.conf ]; then
		loximConf=~/.loxim.conf;
	elif [ -e /etc/loxim.conf ]; then
		loximConf=/etc/loxim.conf
	else 
		echo "no LoXiM configuration file found, exiting";
		exit 1;
	fi 
}


#definition of files created by loxim, all files should be listed here to be sure that 
#testing framework is woking correctly 
findLoximStoreObjects() {
	i=0
	loximStoreFiles[$i]=`grep "store_file_default=" "$loximConf" | cut -f2 -d"="`
	let "i += 1"
	loximStoreFiles[$i]=`grep "store_file_map=" $loximConf | cut -f2 -d"="`
	let "i += 1"
	loximStoreFiles[$i]=`grep "store_file_roots=" $loximConf | cut -f2 -d"="`
	let "i += 1"
	loximStoreFiles[$i]=`grep "store_file_views=" $loximConf | cut -f2 -d"="`
	let "i += 1"
	loximStoreFiles[$i]=`grep "store_file_fclasses=" $loximConf | cut -f2 -d"="`
	let "i += 1"
	loximStoreFiles[$i]=`grep "store_file_interfaces=" $loximConf | cut -f2 -d"="`
	let "i += 1"
	loximStoreFiles[$i]=`grep "store_file_schemas=" $loximConf | cut -f2 -d"="`
	let "i += 1"
	loximStoreFiles[$i]=`grep "logfile=" $loximConf | cut -f2 -d"="`
	let "i += 1"
	loximStoreFiles[$i]=`grep "index_file=" $loximConf | cut -f2 -d"="`
	let "i += 1"
	loximStoreFiles[$i]=`grep "index_file_metadata=" $loximConf | cut -f2 -d"="`
	let "i += 1"
	loximStoreFiles[$i]=`grep "store_file_schemas=" $loximConf | cut -f2 -d"="`
	let "i += 1"
	loximStoreFiles[$i]=`grep "logspath="  $loximConf | cut -f2 -d"="`

}

#cleaning loxim files, because each test should start with empty loxim instance
removeLoximStoreObjects() {
	for ((i = 0; i < ${#loximStoreFiles[@]}; i++ )); do
		#echo ^^^^DEBUG^^^^ REMOVING  ${loximStoreFiles[$i]} 
		rm ${loximStoreFiles[$i]} 2>/dev/null
	done
}


#it assumes that there is only one instance of Loxim server running!!!
findLoximServerPid() {
	loximServerPid=`ps a | grep -v grep | grep loxim_server | sed "s/^[ ]*\([^ ]\)/\1/g" | cut -d" " -f1`
}

#different loxim stats according to test type
#this function has one parameter (not mandatory)
#	*test type
runLoximServer() {
	echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"	
	echo "~~~~~~~~~~~~~~~~~~ starting loxim server ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"	
	$loximKiller  $timeout &
	if [ $# -eq 0 ]; then
		$loximServer &
	elif [ $# -eq 1 ]; then
		case $1 in
			corr) $loximServer & ;;
			leaks) $memcheck $customParameters --log-file=$resultCurrPath $loximServer & ;; 
			leaksPr) HEAPCHECK=normal $loximServer & ;;
			heapPr) ( export HEAP_PROFILE_ALLOCATION_INTERVAL=90000000000 ; export HEAP_PROFILE_INUSE_INTERVAL=90000000000 ; HEAPPROFILE=$resultCurrPath $loximServer ) & ;;
			heap) echo $massif $customParameters --massif-out-file=$resultCurrPath $loximServer; $massif --massif-out-file=$resultCurrPath $loximServer & ;;
			hel) echo  $helgrind $customParameters --log-file=$resultCurrPath $loximServer;  $helgrind $customParameters --log-file=$resultCurrPath $loximServer & ;;
			drd) echo $drd $customParameters --log-file=$resultCurrPath $loximServer;  $drd $customParameters --log-file=$resultCurrPath $loximServer & ;;  
		esac
		#echo ^^^^DEBUG^^^^ timeout = $timeout
	fi
	echo "~~~~~~~~~~~~~~~~~~ loxim server started in background ~~~~~~~~~~~~~~~~~~~~~~~~~~"	
}

#stopping loxim server by executing #shutdown statement
stopLoximServer() {
	echo "~~~~~~~~~~~~~~~~~~~ stopping loxim server ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"	
	$lsbql -l root -P a -m slash -f $LOXIM_HOME/Testing/tests/simpleTests/shutdown.sbql
}

#this function runs simple single test (it assumes that loxim servers is running
#this fucntion has two parameters
#	*test path
#	*test mode
runSingleTest() {
	echo "################################################################################" 
	echo "#################### starting single simple test $testType test path $1 ########" 
	runLoximServer $testType 
	sleep $serverDelay 
	echo mode $mode login $login password $password
	$lsbql -l $login -P $password -m $2 -f $1 >> $outputCurrPath
	findLoximServerPid
	stopLoximServer
	removeLoximStoreObjects 
	wait
	echo "################### single simple test finished ################################" 
}

getLoginAndPass() {
	if [ "$3" != "" ]; then
		login=$3
	fi
	if [ "$4" != "" ]; then
		password=$4
	fi

}

#this function run advanced test. It opens file with test definition (there should be 
#simple test name and test mode). Then it runs all simple tests in one instance of loxim. 
#this function has one parameter (not mandatory)
#	*advanced test name
runSingleAdvancedTest() {
	echo "################################################################################" 
	echo "#################### starting single advanced test $testType test path $1 ######" 
	runLoximServer $testType
	sleep $serverDelay
	cat $1 | while read SIMPLETEST; do
		currentTest=`echo $SIMPLETEST | cut -f1 -d" "`
		getLoginAndPass $SIMPLETEST
		echo "----------------- running simple test $currentTest -----------------------------"
		#echo currentTest $currentTest
		mode=`echo $SIMPLETEST | cut -f2 -d" "`
		echo mode $mode login $login password $password
		$lsbql -l $login -P $password -m $mode -f $LOXIM_HOME"/Testing/tests/simpleTests/"$currentTest >> $outputCurrPath 
	done
	findLoximServerPid
	stopLoximServer
	removeLoximStoreObjects
	wait
	echo "######################### single advanced test finished ########################" 
	   
	
}

#this function is postprocessing test result if there is possibility/need (eg transform
#google perf tools output into easily readable form)
postprocessOutput() {
	echo "~~~~~~~~ postprocessing results from test ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"	
	case $testType in
		heap) ms_print $resultCurrPath > $resultCurrPath"_ms_print" ;;
		leaksPr) echo ten plik ma być!!  `ls /tmp | grep loxim_server | grep $loximServerPid`
						mv /tmp/`ls /tmp | grep loxim_server | grep $loximServerPid` $resultCurrPath".heap"
						pprof --inuse_objects --lines --heapcheck --ps $loximServer $resultCurrPath.heap >  $resultCurrPath".ps";
						pprof --inuse_objects --lines --heapcheck --pdf $loximServer $resultCurrPath.heap >  $resultCurrPath".pdf";
						pprof --inuse_objects --lines --heapcheck --gif $loximServer $resultCurrPath.heap >  $resultCurrPath".gif";
						pprof --inuse_objects --lines --heapcheck --text $loximServer $resultCurrPath.heap >  $resultCurrPath".txt" ;;
		heapPr)	pprof --alloc_space --ps --nodefraction=$nodefraction --edgefraction=$edgefraction $loximServer $resultCurrPath.0001.heap > $resultCurrPath"AllocSpace.ps";
						pprof --alloc_space --pdf --nodefraction=$nodefraction --edgefraction=$edgefraction $loximServer $resultCurrPath.0001.heap > $resultCurrPath"AllocSpace.pdf";
						pprof --alloc_space --text --nodefraction=$nodefraction --edgefraction=$edgefraction $loximServer $resultCurrPath.0001.heap > $resultCurrPath"AllocSpace.text";
						pprof --alloc_space --gif --nodefraction=$nodefraction --edgefraction=$edgefraction $loximServer $resultCurrPath.0001.heap > $resultCurrPath"AllocSpace.gif";
	
						pprof --alloc_objects --ps --nodefraction=$nodefraction --edgefraction=$edgefraction $loximServer $resultCurrPath.0001.heap > $resultCurrPath"AllocObjects.ps";
						pprof --alloc_objects --pdf --nodefraction=$nodefraction --edgefraction=$edgefraction $loximServer $resultCurrPath.0001.heap > $resultCurrPath"AllocObjects.pdf";
						pprof --alloc_objects --text --nodefraction=$nodefraction --edgefraction=$edgefraction $loximServer $resultCurrPath.0001.heap > $resultCurrPath"AllocObjects.text";
						pprof --alloc_objects --gif --nodefraction=$nodefraction --edgefraction=$edgefraction $loximServer $resultCurrPath.0001.heap > $resultCurrPath"AllocObjects.gif";

						pprof --inuse_space --ps --nodefraction=$nodefraction --edgefraction=$edgefraction $loximServer $resultCurrPath.0001.heap > $resultCurrPath"InUseSpace.ps";
						pprof --inuse_space --pdf --nodefraction=$nodefraction --edgefraction=$edgefraction $loximServer $resultCurrPath.0001.heap > $resultCurrPath"InUseSpace.pdf";
						pprof --inuse_space --text --nodefraction=$nodefraction --edgefraction=$edgefraction $loximServer $resultCurrPath.0001.heap > $resultCurrPath"InUseSpace.text";
						pprof --inuse_space --gif --nodefraction=$nodefraction --edgefraction=$edgefraction $loximServer $resultCurrPath.0001.heap > $resultCurrPath"InUseSpace.gif";
	
						pprof --inuse_objects --ps --nodefraction=$nodefraction --edgefraction=$edgefraction $loximServer $resultCurrPath.0001.heap > $resultCurrPath"InUseObjects.ps";
						pprof --inuse_objects --pdf --nodefraction=$nodefraction --edgefraction=$edgefraction $loximServer $resultCurrPath.0001.heap > $resultCurrPath"InUseObjects.pdf";
						pprof --inuse_objects --text --nodefraction=$nodefraction --edgefraction=$edgefraction $loximServer $resultCurrPath.0001.heap > $resultCurrPath"InUseObjects.text";
						pprof --inuse_objects --gif --nodefraction=$nodefraction --edgefraction=$edgefraction $loximServer $resultCurrPath.0001.heap > $resultCurrPath"InUseObjects.gif";;
		corr) echo diff $answerPath $outputCurrPath  $resultCurrPath;  diff $answerPath $outputCurrPath > $resultCurrPath;
					return `wc -l $resultCurrPath | cut -f1 -d" "`;;
	esac
}

#This function has one parametet test name (and it checks test type and class
findAnswerFile() {
	if [ $testType = "corr" ]; then
		if [ $testClass = "advanced" ]; then
			echo advanced
			answerPath=$LOXIM_HOME/Testing/answers/advanced/`echo $1 | cut -f1 -d"."`".txt"
			echo $answerPath
		else 
			echo simple
			answerPath=$LOXIM_HOME/Testing/answers/simple/`echo $1 | cut -f1 -d"."`".txt"
			echo $answerPath
		fi
	fi 
}
 
#this function is core of testing suite
#it is responsible for running test
makeTest() {
	echo "%%%%%%%%%%%%%%%%% STARTING TEST %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
	if [ $allTests = "true" ]; then
		if [ $testClass == "simple" ]; then 
			cat $LOXIM_HOME/Testing/tests/simpleTests/all.txt | while read LINE; do
				makeOuptunName `echo $LINE | cut -f1 -d" "`
				makeResultName `echo $LINE | cut -f1 -d" "`
				findAnswerFile `echo $LINE | cut -f1 -d" "`
				getLoginAndPass $LINE
				runSingleTest $testPath`echo $LINE | cut -f1 -d" "` `echo $LINE | cut -f2 -d" "`
				postprocessOutput
			done
		else
			cat $LOXIM_HOME/Testing/tests/advancedTests/all.txt | while read LINE; do
				makeOuptunName `echo $LINE | cut -f1 -d" "`
				makeResultName `echo $LINE | cut -f1 -d" "`
				findAnswerFile `echo $LINE | cut -f1 -d" "`
				runSingleAdvancedTest $testPath`echo $LINE | cut -f1 -d" "` 
				postprocessOutput
			done
		fi
	else 
		makeOuptunName $testName
		makeResultName $testName
		findAnswerFile $testName
		if [ $testClass == "simple" ]; then 
				runSingleTest $testPath $mode
		else
				runSingleAdvancedTest $testPath 
		fi
		postprocessOutput
		echo lastReturn $lastReturn
		lastReturn=$?
	fi
	#stupid hack because sometimes there info about help in answer file
	#here should be inteligent output comparer 
	if [ lastReturn -lt 3 ]; then
		retValue=0
	fi
	echo "%%%%%%%%%%%%%%%%%%% TEST FINISHED %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
	echo retValue $retValue
	exit $retValue
}





