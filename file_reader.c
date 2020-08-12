#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#include "string.h"
#include "logging.h"
#include "file_reader.h"
#include "grammar.h"

struct file_reader
{
	const char* filename;	
	FILE* file_ptr;
	size_t curr_line;
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
			return NULL;
		}
		
		fr->curr_line = 0;
	}
	return fr;
}


FileReaderStatus fileReaderGetLine(FileReader* fr, char* dst)
{
	assert(fr);
	assert(fr->file_ptr);
	assert(dst);
	
	debug("Reading next line");

	char* line = NULL;
READ:	line = fgets(dst, MAX_LINE_SIZE + 1, fr->file_ptr);
	debug("fgets returned");
	if (!line)
	{
		debug("fgets returned NULL");
		if (feof(fr->file_ptr))
		{
			debug("Reached end of file");
			return FILE_READER_EOF;
		}
		debug("File read error");
		return FILE_READER_ERROR;
	}
	
	size_t length = strlen(line);
	if ('\n' == line[length - 1]) line[length - 1] = 0;
	debug("Successfully read line: %s", line);
	
	if (is_comment(line) || is_whitespace(line))
		goto READ;

	++fr->curr_line;
	return FILE_READER_SUCCESS;
}


const char* fileReaderGetFilename(const FileReader* fr)
{
	return fr->filename;
}

size_t fileReaderGetLineNum(const FileReader* fr)
{
	return fr->curr_line;
}

void fileReaderRewind(FileReader* fr)
{
	assert(fr);
	fseek(fr->file_ptr, 0, SEEK_SET);
	fr->curr_line = 0;
}


void fileReaderDestroy(FileReader* fr)
{
	if (fr)
	{
		fclose(fr->file_ptr);
		free(fr);
	}
}

