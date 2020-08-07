TOP=$(abspath "..")
INCLUDE_DIR=$(TOP)/include/

LIB_BASE_NAME=data_structures
LIB_FULL_NAME=lib$(LIB_BASE_NAME).a
LIB_DIR=$(TOP)/lib/

SUBDIRS=linked_list \
	hash_map \
	$()

CFLAGS=-Wall -ansi -pedantic -std=c99
CFLAGS += $(addprefix -iquote, $(INCLUDE_DIR))
CC=gcc

SOURCES=$(shell find . -type f -iname '*.c' | grep -v test)
OBJECTS=$(foreach x, $(basename $(SOURCES)), $(x).o)

lib: |$(LIB_FULL_NAME)
	#ln -sf $(abspath $(LIB_FULL_NAME)) $(LIB_DIR)

$(LIB_FULL_NAME): $(OBJECTS)
	ar cr $@ $^

%.o: %.c %.h
	$(CC) $(CFLAGS) -iquote $(INCLUDE_DIR) -c $< -o $@

test: subdirs

.PHONY: clean lib all subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	make -C $@ $(MAKECMDGOALS)

clean: subdirs
	rm -f $(LIB_FULL_NAME) $(LIB_DIR)/$(LIB_FULL_NAME)
