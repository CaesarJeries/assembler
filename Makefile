TOP=$(abspath ".")
INCLUDE_DIR=$(TOP)/include/ \
	    $()

CFLAGS=-Wall -ansi -pedantic -std=c99
CC=gcc

SOURCES=$(shell find $(TOP) -type f -iname '*.c')
TEST_SOURCES=$(shell find $(TOP) -type f -iname '*_test.c')
TEST_BASENAMES=$(foreach x, $(basename $(TEST_SOURCES)), $(x))


OBJECTS=$(foreach x, $(basename $(SOURCES)), $(x).o)
DEP = $(OBJECTS:%.o=%.d)

TARGET=assembler

%.o : %.c
	# The -MMD flags additionaly creates a .d file with
	# the same name as the .o file.
	$(CC) $(CFLAGS) -iquote $(INCLUDE_DIR) -MMD -c $< -o $@

-include $(DEPS)

all: include $(OBJECTS)
	$(CC) $(CFLAGS) -iquote $(INCLUDE_DIR) $^ -o $@

%_test: %_test.c %.o
	$(CC) $(CFLAGS) -iquote $(INCLUDE_DIR) $^ -o $@

test: include $(TEST_BASENAMES)
	echo "Building tests: $(TEST_BASENAMES)"

################################################################################
#			Header Files					       #
################################################################################

HEADERS=$(shell find $(TOP) -type f -iname '*.h')

include: | $(HEADERS)
	ln -sf $(HEADERS) $(INCLUDE_DIR)

$(HEADERS): | $(INCLUDE_DIR)

$(INCLUDE_DIR):
	if [ ! -d $@ ]; then mkdir $@; fi


################################################################################
#			Clean					       #
################################################################################


.PHONY: clean

clean:
	rm -f *.d
	rm -f *_test
	rm -f *.o $(TARGET)
	rm -rf $(INCLUDE_DIR)
