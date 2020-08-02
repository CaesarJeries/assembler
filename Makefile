
FLAGS=-Wall -ansi -pedantic -std=c99
GCC=gcc

all: main.c register.h
	$(GCC) $(FLAGS) main.c -o main

.PHONY: clean

clean:
	rm -f *.o *.so

