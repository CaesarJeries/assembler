#ifndef __COMMAND_H__
#define __COMMAND_H__

#define MAX_CMD_SIZE 128
#define MAX_OP_SIZE 128


typedef enum
{
	IMMEDIATE_ADDRESSING = 0,
	DIRECT_ADDRESSING = 1,
	RELATIVE_ADDRESSING = 2,
	REGISTER_ADDRESSING = 3

} AddressingMethod;


AddressingMethod get_addr_method(const char* operand);
int get_operand_num(const char* command);

int parse_command(const char* line,
		  char* cmd_name_out,
		  char* src_op_out,
		  char* dst_op_out);

#endif	// __COMMAND_H__

