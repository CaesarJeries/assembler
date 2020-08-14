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
	const char* basename;
	FILE* file_ptr;
	size_t curr_line;
};

static int get_extension_index(const char* filename)
{
	const char* itr = filename + strlen(filename) - 1;
	while (itr > filename)
	{
		if ('.' == *itr) return itr - filename;
		--itr;
	}

	return -1;
}


static char* resolve_filename(const char* filename)
{
	int idx = get_extension_index(filename);
	if (idx < 0) return strappend(filename, ".as");

	return strdup(filename);
}

static char* get_base_name(const char* filename)
{
	int idx = get_extension_index(filename);
	if (idx < 0) return strdup(filename);

	return strndup(filename, idx - 1);
}


FileReader* fileReaderInit(const char* filename)
{
	FileReader* fr = malloc(sizeof(*fr));
	if (fr)
	{
		fr->filename = resolve_filename(filename);
		fr->basename = get_base_name(filename);
		if (!fr->filename || !fr->basename)
		{
			return NULL;
		}

		debug("Basename: %s, filename: %s", fr->basename, fr->filename);

		fr->file_ptr = fopen(fr->filename, "r");
		if (!fr->file_ptr)
		{
			error("Failed to open file: %s", fr->filename);
			fileReaderDestroy(fr);
			return NULL;
		}
		
		fr->curr_line = 0;
		debug("File reader initialized");
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

const char* fileReaderGetBasename(const FileReader* fr)
{
	return fr->basename;
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
		free((void*) fr->filename);
		free((void*) fr->basename);
		fclose(fr->file_ptr);
		free(fr);
	}
}

