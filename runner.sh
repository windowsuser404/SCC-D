#!/bin/bash
rm results_dump
for i in {1..10}; do
	echo -e "Doing $i percent \n" >> results_dump
	python3 tester.py $i;
	./temp_run.sh $1
done
