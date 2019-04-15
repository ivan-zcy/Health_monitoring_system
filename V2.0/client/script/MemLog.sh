#!/bin/bash
#Using for gathering information of Mem

function Usage() {
	echo "Usage: $0 DyAver"
}

if [[ $# -lt 1 ]]; then
	Usage
	exit 1
fi

source ./PiHealth.conf 2>> /dev/zero

if [[ $? -ne 0 ]]; then
	source ./script/PiHealth.conf
	if [[ $? -ne 0 ]]; then
		exit 2
	fi
fi
#$1 is the  dynamic average of the Mem occupancy
DyAver=$1 	

if [[ x"$DyAver" = x ]]; then
	exit 3
fi

MemValue=(`free -m | head -n 2 | tail -n 1 | awk '{printf("%s %s\n",$2,$3) }'`)
MemAvaPrec=`echo "scale=1;${MemValue[1]}*100/${MemValue[0]}" | bc`
NowAver=`echo "scale=1;0.7*${MemAvaPrec}+0.3*${DyAver}" | bc`


NowTime=`date +"%Y-%m-%d__%H:%M:%S"`
echo "$NowTime ${MemValue[0]}M ${MemValue[1]}M ${MemAvaPrec}% ${NowAver}%" #>> $MemLog
echo ${NowAver}




