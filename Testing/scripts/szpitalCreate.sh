#!/bin/bash

#script have one parameter - how many times repeat szpital-create.sbql
#each repetition make more and more references to objects

loxim_server="../../src/loxim_server"
lsbql="../../src/lsbql"
let x=0

if [ $# -eq 1 ] 
then
	while [ $x -lt $1 ]
	do	
		((x++))
		$lsbql -l root -P aa -m slash -f ../tests/advancedTests/szpital-create.sbql
	done
else
echo "usage: szpitalCreate [numberOfRepetions]"
fi
