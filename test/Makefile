TOP=~/projects/assembler

FILES=$(wildcard *.h)
FILES_ABS=$(abspath $(FILES))
INCLUDE_DIR=$(TOP)/include/

all:
	ln -s $(FILES_ABS) $(INCLUDE_DIR)
.PHONY: clean

clean:
	rm $(addprefix $(INCLUDE_DIR), $(FILES))

