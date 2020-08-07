#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#include "file_reader.h"

struct file_reader
{
	const char* filename;	
	FILE* file_ptr;
};

FileReader* fileReaderInit(const char* filename)
{
	FileReader* fr = malloc(sizeof(*fr));
	if (fr)
	{
		fr->filename = filename;
		fr->file_ptr = fopen(filename, "r");
		if (!fr->file_ptr)
		{
			fileReaderDestroy(fr);
			fr = NULL;
		}
	}
	return fr;
}


FileReaderStatus fileReaderGetLine(FileReader* fr, char** dst)
{
	assert(fr);
	assert(dst);

	char* line = fgets(*dst, MAX_LINE_SIZE + 1, fr->file_ptr);
	if (!line)
	{
		if (feof(fr->file_ptr))
		{
			return FILE_READER_EOF;
		}
		return FILE_READER_ERROR;
	}

	return FILE_READER_SUCCESS;
}


void fileReaderRewind(FileReader* fr)
{
	assert(fr);
	fseek(fr->file_ptr, 0, SEEK_SET);
}


void fileReaderDestroy(FileReader* fr)
{
	if (fr)
	{
		fclose(fr->file_ptr);
		free(fr);
	}
}

