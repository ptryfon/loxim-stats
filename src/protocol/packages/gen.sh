#!/bin/bash

#Skrypcik pomocniczy - na podstawie danego wzorce klasy - tworzy now± o danej nazwie

FROM=ASCOkPackage
TO="$1"

FD=`echo ${FROM} | ./uppercase.sh`
FT=`echo ${TO}   | ./uppercase.sh`

echo ${FD}
echo ${FT}

cat ${FROM}.cpp | replace ${FROM} ${TO} > ${TO}.cpp
cat ${FROM}.h | replace ${FROM} ${TO} | replace ${FD} ${FT} > ${TO}.h