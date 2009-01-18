#!/bin/bash

source lib.sh



#there should be no running loxim server and loxim clients boefore test staring
$loximKiller 1 $loximData
echo "~~~~~~~~~~~~~~~ ALL TESTS USING GOOGLE PERF TOOLS ~~~~~~~~~~~~~~~~~~~~"
rm $loximData
cleanDefault
sleep 1

makeTest "./perfTools.sh ./100conn.sh 100conn" 120
makeTest " ./perfTools.sh ./1closeBy1.sh 1closeBy1" 200
makeTest "./perfTools.sh \"./mamyConnAtOnce.sh 20\" 20atOnce" 220
makeTest "./perfTools.sh \"./szpitalCreate.sh 1\" 1create" 220
makeTest "{ ./perfTools.sh \"./szpitalCreate.sh 1\" 1createFor10Queries; ./perfTools.sh \"./szpitalQuery.sh 10\" 10Queries; }" 500
makeTest "{ ./perfTools.sh \"./szpitalCreate.sh 1\" 1createFor1Query; ./perfTools.sh \"./szpitalQuery.sh 1\" 1Query; }" 400
makeTest "./perfTools.sh \"./szpitalCreate.sh 3\" 3create" 400
makeTest "./perfTools.sh \"./szpitalCreate.sh 5\" 5create" 600
###
