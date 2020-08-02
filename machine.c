#include <stdio.h>
#include "machine.h"

#define MAX_LINE_LENGTH 80
#define BUFFER_SIZE 1024 * (1 + MAX_LINE_LENGTH)


struct machine
{
	int registers[8];
	int pc;
	int psw;
	int ic; // instruction counter
	int dc; // data counter
	FILE* input;
};


Machine* machineInit(const char* input_filename)
{
	FILE* file = fopen(input_filename, "r");
	if (!file)
	{
		fprintf(stderr,
			"Failed to open input file: %s: %s",
			input_filename,
			strerror(errno));
		return -1;
	}

	Machine* machine = calloc(1, sizeof(*machine));
	if (!machine)
	{
		fclose(file);
	}
	machine.input = file;
	machine.ic = 100;
	machine.dc = 0;

	return machine;
}


int machineExecute(Machine* machine)
{
	static char* buffer[BUFFER_SIZE] = {0};
	while (!feof(machine.input))
	{
		int read_bytes = fread(buffer, sizeof(char), BUFFER_SIZE, machine.input);
		int symbol_declared = 0;
		for (int i = 0; i < read_bytes; ++i)
		{
			const char* expr = buffer[i];
			if (is_comment(expr))
			{
				continue;
			}
			if (is_symbol(expr))
			{
				symbol_declared = 1;
				if (is_data(expr) || is_string(expr))
				{
					// insert symbol to data table.
					// notify on error
					symbolTableInsert(machine, expr);
				}
			}
			// identify type and insert to table or

			if (is_entry(expr))
			{
				// skip. this is handled in the second pass.
			}
			if (is_extern(expr))
			{
				//insert to externals table
			}







void machineDestroy(Machine* machine)
{
	free(machine);
}

