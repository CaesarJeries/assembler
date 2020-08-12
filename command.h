#ifndef __COMMAND_H__
#define __COMMAND_H__

#define ADDR_METHOD_NUM 4
#define MAX_CMD_SIZE 128
#define MAX_OP_SIZE 128

typedef struct
{
	int op_code;
	int funct;
	const char* name;
	int has_src;
	int has_dst;
	int src_valid_addressing[ADDR_METHOD_NUM];
	int dst_valid_addressing[ADDR_METHOD_NUM];

} Command;

typedef enum
{
	IMMEDIATE_ADDRESSING = 0,
	DIRECT_ADDRESSING = 1,
	RELATIVE_ADDRESSING = 2,
	REGISTER_ADDRESSING = 3

} AddressingMethod;


typedef enum
{
	OP_CODE_OFFSET = 0,
	SRC_ADDR_OFFSET = 6,
	SRC_REG_OFFSET = 8,
	DST_ADDR_OFFSET = 11,
	DST_REG_OFFSET = 13,
	FUNCT_OFFSET = 16,
	A_OFFSET = 21,
	R_OFFSET = 22,
	E_OFFSET = 23

} Offsets;

Command get_command_definition(const char* command_name);

AddressingMethod get_addr_method(const char* operand);
int is_register(const char* operand);
int get_register_number(const char* operand);
int get_value(const char* operand);
char* get_label(const char* operand);

int get_operand_num(const char* command);

int parse_command(const char* line,
		  char* cmd_name_out,
		  char* src_op_out,
		  char* dst_op_out);

#endif	// __COMMAND_H__

