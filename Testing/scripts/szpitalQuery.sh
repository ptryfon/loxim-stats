#!/bin/bash

#script have one parameter - how many times repeat szpital-query.sbql
#it can improve visibility of allocations made by query

loxim_server="../../src/loxim_server"
lsbql="../../src/lsbql"
let x=0

if [ $# -eq 1 ] 
then
	while [ $x -lt $1 ]
	do	
		((x++))
		$lsbql -l root -P aa -m slash -f ../tests/advancedTests/szpital-query.sbql
	done
else
	echo "usage szpitalQuery [numberOfRepetitions]"
fi
