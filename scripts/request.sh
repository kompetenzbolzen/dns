#!/bin/bash

while true; do
	dig +short -p 5333 test.example.com @localhost || break
done
