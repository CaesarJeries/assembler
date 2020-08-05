# todo: define TOP in a single location
TOP=~/projects/assembler

FLAGS=-Wall -ansi -pedantic -std=c99
GCC=gcc
FILES=$(wildcard *.h)
FILES_ABS=$(abspath $(FILES))
INCLUDE_DIR=$(TOP)/include/

all:
	ln -s $(FILES_ABS) $(INCLUDE_DIR)

%.o: %.c %.h
	$(GCC) $(FLAGS) -iquote $(INCLUDE_DIR) -c $< -o $@

%_test: %_test.c %.o 
	$(GCC) $(FLAGS) -iquote $(INCLUDE_DIR) $^ -o $@


.PHONY: clean

clean:
	rm -f *.o *_test

