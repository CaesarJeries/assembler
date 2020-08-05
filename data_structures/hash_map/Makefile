TOP=~/projects/assembler

FILES=$(wildcard *.h)
FILES_ABS=$(abspath $(FILES))
INCLUDE_DIR=$(TOP)/include/

LIB_BASE_NAME=data_structures
LIB_FULL_NAME=lib$(LIB_BASE_NAME).a

SOURCES=$(shell find . -type f -iname '*.c' | grep -v test)
TEST_SOURCES=$(shell find . -type f -iname '*_test.c')
TEST_BASENAMES=$(foreach x, $(basename $(TEST_SOURCES)), $(x))

CFLAGS=-Wall -ansi -pedantic -std=c99
CFLAGS += $(addprefix -iquote, $(INCLUDE_DIR))
CC=gcc

OBJECTS=$(foreach x, $(basename $(SOURCES)), $(x).o)


%_test: %_test.c %.o
	$(CC) $(CFLAGS) $^ -o $@


.PHONY: clean

clean:
	rm -f *.a
	rm -f *.d
	rm -f *.o *_test

