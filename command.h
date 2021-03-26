#ifndef __COMMAND_H__
#define __COMMAND_H__

#define MAX_CMD_SIZE 128
#define MAX_OP_SIZE 128


typedef enum
{
	IMMEDIATE_ADDRESSING = 0,
	DIRECT_ADDRESSING = 1,
	RELATIVE_ADDRESSING = 2,
	REGISTER_ADDRESSING = 3,
	ADDR_METHOD_NUM // must be last

} AddressingMethod;

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
	OP_CODE_OFFSET = 0,
	FUNCT_OFFSET = 4,
	SRC_ADDR_OFFSET = 8,
	DST_ADDR_OFFSET = 10,

} Offsets;

Command get_command_definition(const char* command_name);

AddressingMethod get_addr_method(const char* operand);
const char* get_addr_method_name(AddressingMethod method);
int is_register(const char* operand);
int get_register_number(const char* operand);
int get_value(const char* operand);
char* get_label(const char* operand);

int get_operand_num(const char* command);

int parse_command(const char* line,
		  char* cmd_name_out,
		  char* src_op_out,
		  char* dst_op_out);

int is_command(const char* label);

#endif	// __COMMAND_H__

