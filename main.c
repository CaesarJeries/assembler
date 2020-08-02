#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "machine.h"

int main(int argc, const char **argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "No input files were supplied.\n");
		return -1;
	}

	int handled_files = 0;
	int total_files = argc - 1;

	while (handled_files < total_files)
	{
		++handled_files;
		const char* input_filename = argv[handled_files];
		printf("Handling input file: %s\n", input_filename);
		Machine* machine = machineInit(input_filename);

		machineExecute(machine);
		machineDestroy(machine);
	}

	return 0;
}

/**
 *
 * output:
 * symbol table: symbol, value, type
 * entries: name, value
 * externals: name, address
 *
 **/
