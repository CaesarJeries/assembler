#ifndef  __REGISTER_H__
#define  __REGISTER_H__


typedef unsigned int Value;


typedef struct {
	const char* name;
	Value value;
} Register;




typedef union {
	Instruction i;
	Value value;
	int is_inst;
} Word;


#endif //  __REGISTER_H__
