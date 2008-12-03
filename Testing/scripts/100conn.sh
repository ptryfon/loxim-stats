#!/bin/bash
let max=100
let x=0
loxim_server="../../src/loxim_server"
lsbql="../../src/lsbql"
while [ $x -lt $max ]
do
	((x++))
	$lsbql -l root -P aa -m slash -f ../tests/simpleTests/quit.sbql
done

