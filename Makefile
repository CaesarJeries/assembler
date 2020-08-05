TOP=$(abspath ".")
INCLUDE_DIR=$(TOP)/include/ \
	    $()

LIB_DIR=$(TOP)/lib/ \
	$()

SUBDIRS=data_structures\
	$()

CFLAGS=-Wall -ansi -pedantic -std=c99
CFLAGS += $(addprefix -iquote, $(INCLUDE_DIR))
CC=gcc

SOURCES=$(shell find $(TOP) -type f -iname '*.c' | grep -v test)
TEST_SOURCES=$(shell find $(TOP) -type f -iname '*_test.c')
TEST_BASENAMES=$(foreach x, $(basename $(TEST_SOURCES)), $(x))

OBJECTS=$(foreach x, $(basename $(SOURCES)), $(x).o)
DEP = $(OBJECTS:%.o=%.d)

TARGET=assembler

.PHONY: clean subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	make -C $@ $(MAKECMDGOALS)


%.o : %.c
	# The -MMD flags additionaly creates a .d file with
	# the same name as the .o file.
	$(CC) $(CFLAGS) -MMD -c $< -o $@

-include $(DEPS)

all: include lib subdirs $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

%_test: %_test.c %.o
	$(CC) $(CFLAGS) $^ -o $@

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
#			Libraries					       #
################################################################################

lib: |$(LIB_DIR)
	$(foreach d, $(SUBDIRS), make lib -C $(d))

$(LIB_DIR):
	if [ ! -d $@ ]; then mkdir $@; fi

################################################################################
#			Clean					       	       #
################################################################################



clean:
	rm -f *.d
	rm -f *_test
	rm -f *.o $(TARGET)
	rm -rf $(INCLUDE_DIR)
	rm -rf $(LIB_DIR)
