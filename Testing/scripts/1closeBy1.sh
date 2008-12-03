#!/bin/bash

#there should be only one lsbql client running during this test

loxim_server="../../src/loxim_server"
lsbql="../../src/lsbql"

$lsbql -l root -P aa -m slash -f ../tests/simpleTests/neverending1.sbql &
pid=`ps a | grep -v grep | grep lsbql | sed "s/^[ ]*\([^ ]\)/\1/g" | cut -d" " -f1`
sleep 30
#echo $pid
kill -9 $pid
