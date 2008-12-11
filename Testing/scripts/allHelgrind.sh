#!/bin/bash

$loximKiller 1 $loximData
echo "~~~~~~~~~~~~~~~ ALL TESTS USING HELGRIND ~~~~~~~~~~~~~~~~~~~~"
rm $loximData
cleanDefault
sleep 1


makeTest "./helgrind.sh ./100conn.sh 100conn" 60
makeTest "./helgrind.sh ./1closeBy1.sh 1closeBy1" 120
makeTest "./helgrind.sh \"./mamyConnAtOnce.sh 20\" 20atOnce" 60
makeTest "./helgrind.sh \"./szpitalCreate.sh 1\" 1create" 500
makeTest "{ ./helgrind.sh \"./szpitalCreate.sh 1\" 1createFor1Query; ./helgrind.sh \"./szpitalQuery.sh 1\" 1Query; }" 900
