#!/bin/bash

while true
do
	dd if=/dev/urandom bs=$(echo $RANDOM/100 | bc) count=1 2>/dev/null | nc -cu localhost 53 > /dev/null
done
