SHELL := /bin/bash

compile:
	mpicc main.c -o gray -lm

run:
	mpirun -np 4 gray ./Images/720p.png ./Output/720p.png

test:
	sh test.sh