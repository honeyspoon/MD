CC=g++

CFLAGS=-std=c++17 -pedantic -Wall -Wextra
LFLAGS=-mbig-endian

main: main.cpp
	$(CC) $(CFLAGS) main.cpp -o main

run: main
	./main
