TOP=$(abspath .)

INCLUDE_DIR=.\
	    test\
	    $()

CFLAGS=-Wall -ansi -pedantic -std=c99 -DNDEBUG
CFLAGS += $(addprefix -iquote, $(INCLUDE_DIR))
CC=gcc

SOURCES=$(shell find $(TOP) -type f -iname '*.c' | grep -v test | grep -v main.c)
OBJECTS=$(foreach x, $(basename $(SOURCES)), $(x).o)

TEST_SOURCES=$(shell find $(TOP) -type f -iname '*_test.c')
TEST_BASENAMES=$(foreach x, $(basename $(TEST_SOURCES)), $(x))

DEPS=$(OBJECTS:%.o=%.d)

TARGET=assembler

all: main.o $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $(TARGET)

%.o : %.c
	@# The -MMD flags additionaly creates a .d file with
	@# the same name as the .o file.
	$(CC) $(CFLAGS) -MMD -c $< -o $@

-include $(DEPS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@


%_test: %_test.c
	$(CC) $(CFLAGS) $^ $(OBJECTS) -o $@

test: $(OBJECTS) $(TEST_BASENAMES)
	echo "Building tests: $(TEST_BASENAMES)"

TEST_LOGFILE=tests.log

run_test: test
	echo "Executing tests: $(TEST_BASENAMES)"
	$(foreach t, $(TEST_BASENAMES), $(shell echo $(t); $(t) >> $(TEST_LOGFILE)))
	cat $(TEST_LOGFILE)


sub: clean
	tar -cf assembler.tar `ls -A | grep -v git`


.PHONY: all clean test run_test sub

clean:
	rm -f *.tar
	rm -f test/end-to-end/*.ent
	rm -f test/end-to-end/*.ext
	rm -f test/end-to-end/*.ob
	rm -f *.d
	rm -f *_test
	rm -f *.o
	rm -f $(TARGET)

