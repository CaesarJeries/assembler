#include <stdio.h>

#include "assembler.h"
#include "logging.h"


int main(int argc, const char **argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "No input files were supplied.\n");
		return -1;
	}

	int handled_files = 0;
	int total_files = argc - 1;

	Assembler* assembler = assemblerInit();
	while (handled_files < total_files)
	{
		assemblerReset(assembler);
		++handled_files;
		const char* input_filename = argv[handled_files];
		
		debug("Handling input file: %s", input_filename);
		int status = assemblerProcess(assembler, input_filename);
		if (0 != status)
		{
			assemblerDestroy(assembler);
			return -1;
		}
	}

	assemblerDestroy(assembler);
	return 0;
}

