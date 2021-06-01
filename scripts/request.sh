#!/bin/bash

while true; do
	dig test.example.com @localhost | sed -e '/;.*/d' -e '/^$/d'
done
