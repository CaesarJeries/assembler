#include <assert.h>
#include <errno.h>
#include <malloc.h>
#include <stddef.h>	// size_t

#include "assembler.h"
#include "file_reader.h"
#include "grammar.h"
#include "hash_map.h"
#include "linked_list.h"
#include "logging.h"
#include "parser.h"
#include "string.h"

#define MAX_FILENAME_LENGTH 128


struct assembler
{
	size_t data_counter;
	size_t inst_counter;
	
	HashMap* sym_table;
	HashMap* ext_table;
	HashMap* ent_table;
};


static size_t str_hash(void* ptr, size_t size)
{
	const char* str = ptr;
	size_t hash = 5381;
	int c = 0;

	while (*str)
	{
		c = *str;
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
		++str;
	}

	return hash % size;
}


static void* str_copy(void* s)
{
	return strdup(s);
}

static void str_free(void* s)
{
	free(s);
}

static int str_cmp(void* s1, void* s2)
{
	return strcmp(s1, s2);
}


Assembler* assemblerInit()
{
	Assembler* assembler = malloc(sizeof(*assembler));
	if (assembler)
	{
		assembler->data_counter = 0;
		assembler->inst_counter = 0;

		HashMapEntryHandlers handlers = {str_copy, str_free,
						 str_copy, str_free};
		assembler->sym_table = hashMapInit(str_hash, str_cmp, handlers);
		assembler->ext_table = hashMapInit(str_hash, str_cmp, handlers);
		assembler->ent_table = hashMapInit(str_hash, str_cmp, handlers);
		
		if (!assembler->sym_table || !assembler->ext_table || !assembler->ent_table)
		{
			assemblerDestroy(assembler);
			assembler = NULL;
		}
	}

	return assembler;
}


char* search_for_label(char* line, char** label)
{
	// todo: implement
	return line;
}


static void /* todo: change retval */ parseLine(Assembler* assembler, FileReader* fr, char* line)
{
	debug("Parsing line: %s", line);
	char* label = NULL;
	char* itr = search_for_label(line, &label);

	if (is_data(line))
	{
		debug("Data directive detected");
		char* error_msg = NULL;	
		LinkedList* data_list = parse_data(line, &error_msg);
		if (!data_list)
		{
			report_error(error_msg);
			return;
		}

		size_t list_size = linkedListSize(data_list);
#ifndef NDEBUG
		printf("Read %lu data elements\n", list_size);
		for (size_t i = 0; i < list_size; ++i)
		{
			const char* element = linkedListGetAt(data_list, i);
			printf("Element[%lu]: %s\n", i, element);

		}

#endif	// NDEBUG
		linkedListDestroy(data_list);
	}
}

static int firstPass(Assembler* assembler, FileReader* fr)
{
	assert(assembler);
	assert(fr);

	debug("Starting first pass");
	FileReaderStatus status = FILE_READER_SUCCESS;
	while (1)
	{
		static char line[MAX_LINE_SIZE + 1] = {0};
		status = fileReaderGetLine(fr, line);
		debug("Read line. Checking status.");
		if (FILE_READER_SUCCESS != status)
		{
			debug("File reader failed to read line");
			if (FILE_READER_EOF == status)
			{
				debug("Reached end of file");
				return ASSEMBLER_SUCCESS;
			}
			debug("aaaa");
			const char* filename = fileReaderGetFilename(fr);
			error("Failed to read from file: %s: %s\n", filename, strerror(errno));
			fileReaderDestroy(fr);
			return ASSEMBLER_FILE_READ_ERR;
		}
		parseLine(assembler, fr, line);
		break;
	}
	return 0;
}


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


static void secondPass(Assembler* assembler, FileReader* fr /* this param necessary? */)
{
	;
}


AssemblerStatus assemblerProcess(Assembler* assembler, const char* filename)
{
	debug("Processing file: %s", filename);
	
	if (!is_valid_extension(filename))
	{
		error("Invalid file extension: %s", filename);
		return ASSEMBLER_INVALID_EXT;
	}

	debug("File has a valid extension");

	FileReader* fr = fileReaderInit(filename);
	if (!fr)
	{
		error("Failed to opon file: %s: %s", filename, strerror(errno));
		return ASSEMBLER_FILE_OPEN_ERR;
	}

	debug("File reader initialized");

	if (0 == firstPass(assembler, fr))
	{
		fileReaderRewind(fr);
		secondPass(assembler, fr);
		fileReaderDestroy(fr);
		return ASSEMBLER_SUCCESS;
	}
	
	fileReaderDestroy(fr);
	return ASSEMBLER_PARSE_ERR;
}


void assemblerReset(Assembler* assembler)
{
	assert(assembler);
	
	assembler->data_counter = 0;
	assembler->inst_counter = 0;

	hashMapClear(assembler->sym_table);
	hashMapClear(assembler->ext_table);
	hashMapClear(assembler->ent_table);
}


void assemblerDestroy(Assembler* assembler)
{
	if (assembler)
	{
		hashMapDestroy(assembler->sym_table);
		hashMapDestroy(assembler->ext_table);
		hashMapDestroy(assembler->ent_table);
		free(assembler);
	}
}

