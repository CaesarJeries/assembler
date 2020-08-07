#include <assert.h>
#include <errno.h>
#include <malloc.h>
#include <stddef.h>	// size_t

#include "assembler.h"
#include "string.h"

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

static int
	FileReaderStatus status = FILE_READER_SUCCESS;
	while (1)
	{
		static char line[MAX_LINE_SIZE + 1] = {0};
		status = fileReaderGetLine(fr, &line);
		if (FILE_READER_SUCCESS != status)
		{
			fileReaderDestroy(fr);
			if (FILE_READER_EOF == status)
			{
				return ASSEMBLER_SUCCESS;
			}
			printf("Failed to read from file: %s: %s\n", filename, strerror(errno));
			return ASSEMBLER_READ_ERROR;
		}

		parseLine(assembler, fr, line);
	}

AssemblerStatus assemblerProcess(Assembler* assembler, const char* filename)
{
	if (!is_valid_extension(filename))
	{
		printf("Invalid file extension: %s\n", filename);
		return ASSEMBLER_INVALID_EXT;
	}

	FileReader* fr = fileReaderInit(filename);
	if (!fr)
	{
		printf("Failed to opon file: %s: %s\n", filename, strerror(errno));
		return ASSEMBLER_FILE_OPEN_ERR;
	}

	if (0 == firstPass(assembler, fr))
	{
		fileReaderRewind(fr);
		secondPass(assembler, fr);
		return ASSEMBLER_SUCCESS;
	}

	return ASSEMBLER_PARSE_ERR;
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

