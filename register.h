#ifndef  __REGISTER_H__
#define  __REGISTER_H__


typedef unsigned int Value;


typedef struct {
	const char* name;
	Value value;
} Register;


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


typedef union {
	Instruction i;
	Value value;
	int is_inst;
} Word;

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


#endif //  __REGISTER_H__
