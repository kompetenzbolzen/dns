#!/bin/bash

CRASHCNT=0
GOODCRASHES=0
PROG=build/dnsd

make chaos

ulimit -S -c unlimited $PROG
mkdir -p dumps

while true; do
	$PROG -p 5333 -z tests/zone.file > /dev/null &
	PID=$!

	for i in $(seq 50); do
		dig -p 5333 test.example.com @localhost > /dev/null &
	done

	sleep 5

	if ! kill -s 15 $PID ; then
		CRASHCNT=$(( CRASHCNT + 1 ))

		echo -n "Crash #$CRASHCNT"

		if ! coredumpctl -o dumps/$CRASHCNT.core dump COREDUMP_PID=$PID "COREDUMP_EXE=$(realpath $PROG)" > /dev/null 2>&1; then
			GOODCRASHES=$(( GOODCRASHES + 1 ))
			echo " Good crash #$GOODCRASHES"
		fi
	fi
done
