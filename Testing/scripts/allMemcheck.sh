#!/bin/bash


source lib.sh
#there should be no running loxim server and loxim clients boefore test staring
$loximKiller 1 $loximData
echo "~~~~~~~~~~~~~~~ ALL TESTS USING Valgrind Memcheck ~~~~~~~~~~~~~~~~~~~~"
rm $loximData 2> /dev/null
cleanDefault
sleep 1

makeTest "./memcheck.sh ./100conn.sh 100conn" 60
makeTest "./memcheck.sh ./1closeBy1.sh 1closeBy1" 120
makeTest "./memcheck.sh \"./mamyConnAtOnce.sh 20\" 20atOnce" 80
makeTest "./memcheck.sh \"./szpitalCreate.sh 1\" 1create" 220
makeTest "{ ./memcheck.sh \"./szpitalCreate.sh 1\" 1createFor10Queries; ./memcheck.sh \"./szpitalQuery.sh 10\" 10Queries; }" 800
makeTest "{ ./memcheck.sh \"./szpitalCreate.sh 1\" 1createFor1Query; ./memcheck.sh \"./szpitalQuery.sh 1\" 1Query; }" 400
makeTest "./memcheck.sh \"./szpitalCreate.sh 3\" 3create" 600
makeTest "./memcheck.sh \"./szpitalCreate.sh 5\" 5create" 1000
#