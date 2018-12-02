# This is the makefile for ECE1387 Assignment 3 Branch & Bound Partitioner
# Developer: 	Louis Chen (1000303502)
# Date:		Nov 29, 2018

all:
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -o main.o main.cpp
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -o Partitioner.o Partitioner.cpp
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -o Block.o Block.cpp
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -o Net.o Net.cpp
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -o Solution.o Solution.cpp
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -o Display.o Display.cpp
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++0x -o graphics.o graphics.cpp
	g++ -o Partitioner-LouisChen main.o Partitioner.o Block.o Net.o Solution.o Display.o graphics.o -lX11

clean:
	rm -f main.o Partitioner.o Block.o Net.o Solution.o Display.o graphics.o Partitioner-LouisChen
