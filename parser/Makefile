# todo: define TOP in a single location
TOP=~/projects/assembler

FLAGS=-Wall -ansi -pedantic -std=c99
GCC=gcc

INCLUDE=$(TOP)/include/

%.o: %.c %.h
	$(GCC) $(FLAGS) -I $(INCLUDE) -c $< -o $@

%_test: %_test.c %.o  
	$(GCC) $(FLAGS) -I $(INCLUDE) $^ -o $@


.PHONY: clean

clean:
	rm -f *.o *_test

