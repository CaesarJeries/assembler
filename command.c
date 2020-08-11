#include <assert.h>
#include <stddef.h>	// size_t

#include "command.h"
#include "logging.h"
#include "string.h"

#define ARRAY_SIZE(a) ((size_t) (sizeof(a)/ sizeof(a[0])))

#define ADDR_METHOD_NUM 4

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


static Command error_command = {-1};


static Command commands[] = {
	{0, 0, "mov", 1, 1, {1, 1, 0, 1}, {0, 1, 0, 1}},
	{1, 0, "cmp", 1, 1, {1, 1, 0, 1}, {1, 1, 0, 1}},
	{2, 1, "add", 1, 1, {1, 1, 0, 1}, {0, 1, 0, 1}},
	{3, 2, "sub", 1, 1, {1, 1, 0, 1}, {0, 1, 0, 1}},

	{4, 0, "lea", 1, 1, {0, 1, 0, 0}, {0, 1, 0, 1}},

	{5, 1, "clr", 0, 1, {0}, {0, 1, 0, 1}},
	{6, 2, "not", 0, 1, {0}, {0, 1, 0, 1}},
	{7, 3, "inc", 0, 1, {0}, {0, 1, 0, 1}},
	{8, 4, "dec", 0, 1, {0}, {0, 1, 0, 1}},
	{9, 1, "jmp", 0, 1, {0}, {0, 1, 1, 0}},
	{10, 2, "bne", 0, 1, {0}, {0, 1, 1, 0}},
	{11, 3, "jsr", 0, 1, {0}, {0, 1, 1, 0}},
	{12, 0, "red", 0, 1, {0}, {0, 1, 0, 1}},
	
	{13, 0, "prn", 0, 1, {0}, {1, 1, 0, 1}},
	
	{14, 0, "rts", 0, 0, {0}, {0}},
	{15, 0, "stop", 0, 0, {0}, {0}},
};


static Command get_command_definition(const char* command_name)
{
	debug("Searching for command definition: %s", command_name);
	for (size_t i = 0; i < ARRAY_SIZE(commands); ++i)
	{
		if (0 == strcmp(command_name, commands[i].name))
		{ return commands[i];
		}
	}

	return error_command;
}

static int is_register(const char* op)
{
	// Valid registers are r0 - r7
	int is_reg = ('r' == op[0]);
	is_reg &= (op[1] <= '7' && op[1] >= '0');
	return is_reg;
}


AddressingMethod get_addr_method(const char* operand)
{
	if ('#' == operand[0]) return IMMEDIATE_ADDRESSING;
	if ('&' == operand[0]) return RELATIVE_ADDRESSING;

	if (is_register(operand)) return REGISTER_ADDRESSING;

	return DIRECT_ADDRESSING; // Assume direct addressing by default
}


static int is_addressing_valid(Command cmd,
			       const char* src_op,
			       const char* dst_op)
{
	int addressing_method = get_addr_method(dst_op);
	if (0 == cmd.src_valid_addressing[addressing_method])
	{
		error("Invalid addressing method for the source operand: %s", src_op);
		return 0;
	}
	
	addressing_method = get_addr_method(dst_op);
	if (0 == cmd.dst_valid_addressing[addressing_method])
	{
		error("Invalid addressing method for the destination operand: %s", dst_op);
		return 0;
	}

	return 1;
}


static int is_command_valid(const char* command_name,
			    const char* src_op,
			    const char* dst_op,
			    int op_num)
{
	Command cmd = get_command_definition(command_name);

	if (cmd.op_code < 0)
	{
		error("Illegal command: %s", command_name);
		return 0;
	}

	int expected_op_num = get_operand_num(command_name);
	if (expected_op_num != op_num)
	{
		error("Invalid number of operands. Expected %d, recieved: %d",
		      expected_op_num,
		      op_num);
		return 0;
	}

	return is_addressing_valid(cmd, src_op, dst_op);
}


int parse_command(const char* line,
		  char* cmd_name_out,
		  char* src_op_out,
		  char* dst_op_out)
{
	assert(cmd_name_out);
	assert(src_op_out);
	assert(dst_op_out);

	debug("Parsing instruction from line: %s", line);
	
	static char command_name[MAX_CMD_SIZE] = {0};
	static char src_op[MAX_OP_SIZE] = {0};
	static char dst_op[MAX_OP_SIZE] = {0};

	int read_items = sscanf(line,
			" %s %[^ \t\v,] , %s ",
			command_name,
			src_op,
			dst_op);
	debug("Cmd: %s. Source Op: %s. Dest Op: %s", command_name, src_op, dst_op);
	assert(read_items > 0);

	int read_ops = read_items - 1;	// number of operands read from line
	if (!is_command_valid(command_name, src_op, dst_op, read_ops))
	{
		return -1;
	}
	
	strcpy(cmd_name_out, command_name);
	strcpy(src_op_out, src_op);
	strcpy(dst_op_out, dst_op);

	return 0;
}

int get_operand_num(const char* command)
{
	Command cmd_def = get_command_definition(command);
	if (cmd_def.op_code < 0) return -1;
	return cmd_def.has_src + cmd_def.has_dst;
}
