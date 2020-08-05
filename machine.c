#include <stdio.h>
#include "machine.h"

#define MAX_LINE_LENGTH 80


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


static int process_line(machine, line);
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
	}
	return -1; // todo: implement
}


int machineExecute(Machine* machine)
{
	static char line[MAX_LINE_LENGTH + 1] = {0};
	while (1)
	{
		char* str = fgets(line, max_file_line, machine->input);
		if (!str) break;
		//process_line(machine, line); // this could be problematic when handling instructions spanning multiple lines
	}

	return 0;
}






void machineDestroy(Machine* machine)
{
	free(machine);
}

