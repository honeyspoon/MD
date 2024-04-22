CC=g++

CFLAGS=-std=c++17 -pedantic -Wall -Wextra

main: main.cpp
	$(CC) $(CFLAGS) main.cpp -o main

run: main
	./main
