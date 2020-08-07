#include <assert.h>
#include <stddef.h>	// size_t
#include <malloc.h>

#include "assembler.h"

#define MAX_FILENAME_LENGTH 128



struct assembler
{

};


static int is_valid_extension(const char* filename)
{
	size_t length = strlen(filename);
	for (const char* itr = filename + length -1; itr > filename; --itr)
	{
		if ('.' == *itr)
		{
			return (0 == strcmp(itr, ".as"));
		}
	}

	// file has no extension, and therefor valid
	return 1;
}


Assembler* assemblerInit()
{
	Assembler* assembler = malloc(sizeof(*assembler));
	if (assembler)
	{
		//todo: init
	}

	return assembler;
}


AssemblerStatus assemblerProcess(Assembler* assembler, const char* filename)
{
	if (!is_valid_extension(filename))
	{
		printf("Invalid file extension: %s\n", filename);
		return -1;
	}
}

void assemblerReset(Assembler* assembler)
{
	// todo
}


void assemblerDestroy(Assembler* assembler)
{
	if (assembler)
	{
		; // todo
	}
}

