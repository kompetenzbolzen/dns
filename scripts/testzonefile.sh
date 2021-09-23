#!/bin/bash

if [ "$1" = "test" ]; then
	for i in $(seq 250); do
		echo "$i"
		for o in $(seq 10); do
			HOST="t$i-$o.example.com."
			dig -p 5333 +short "$HOST" @localhost > /dev/null &
		done
		sleep 1
	done
elif [ "$1" = gen ]; then
	for i in $(seq 250); do
		for o in $(seq 10); do
			HOST="t$i-$o.example.com."
			IP="10.10.$i.$o"
	
			echo "$HOST 3600 IN A $IP"
		done
	done
else
	echo "Usage $0 gen|test"
fi
