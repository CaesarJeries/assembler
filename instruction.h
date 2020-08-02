#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include "instruction_handlers.h"

static const char* const instruction_map [] = 
{
	"mov",
	"cmp",
	"add",
	"sub",
	"lea",
	"clr",
	"not",
	"inc",
	"dec",
	"jmp",
	"bne",
	"jsr",
	"red",
	"prn",
	"rts",
	"stop"
};


typedef struct {
	int op_code;
	int source_address;
	int source_reg;
	int dst_address;
	int dst_reg
	int funct;
	int A;
	int R;
	int E;
} Instruction;

Instruction instGet(unsigned int address);


#endif //  __INSTRUCTION_H__
