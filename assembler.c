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

typedef int (*line_type_t) (const char*);
typedef void* (*type_handler_t)(void* params);

typedef struct
{
	line_type_t type;
	type_handler_t handler;

} ParsingUnit;


//static ParsingUnit parsing_unit_handlers[] = {{is_data, parse_data}, {is_string, parse_string}, {is_instruction, parse_instruction}};

#define ARRAY_SIZE(a) ((size_t)sizeof(a)/sizeof(a[0]))


struct assembler
{
	size_t data_counter;
	size_t inst_counter;

	HashMap* data_table;	
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

static size_t int_hash(void* n, size_t size)
{
	return (*(size_t*) n) % size;
}

static int int_cmp(void* a, void* b)
{
	int first = *(int*) a;
	int second = *(int*) b;

	return first - second;
}


static void* int_copy(void* n)
{
	int* new_value = malloc(sizeof(int));
	if (new_value)
	{
		*new_value = *(int*) n;
	}
	return new_value;
}

static void int_free(void* n)
{
	free(n);
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

typedef struct
{
	size_t value;
	char type;

} Symbol;


static void* symbolCopy(void* other)
{
	Symbol* new_symbol = malloc(sizeof(*new_symbol));
	if (new_symbol)
	{
		*new_symbol = *(Symbol*) other;
	}
	
	return new_symbol;
}

static void symbolFree(void* s)
{
	free(s);
}

static void* list_copy(void* list)
{
	return linkedListCopy(list);
}

static void list_free(void* list)
{
	linkedListDestroy(list);
}


Assembler* assemblerInit()
{
	Assembler* assembler = malloc(sizeof(*assembler));
	if (assembler)
	{
		assembler->data_counter = 0;
		assembler->inst_counter = 100;
		
		HashMapEntryHandlers data_handlers = {int_copy, int_free,
						      list_copy, list_free};

		assembler->data_table = hashMapInit(int_hash, int_cmp, data_handlers);

		HashMapEntryHandlers symbol_handlers = {str_copy, str_free,
							symbolCopy, symbolFree};
		assembler->sym_table = hashMapInit(str_hash, str_cmp, symbol_handlers);
		
		HashMapEntryHandlers handlers = {str_copy, str_free,
						 str_copy, str_free};
		assembler->ext_table = hashMapInit(str_hash, str_cmp, handlers);
		assembler->ent_table = hashMapInit(str_hash, str_cmp, handlers);
		
		if (!assembler->data_table ||
		    !assembler->sym_table ||
		    !assembler->ext_table ||
		    !assembler->ent_table)
		{
			assemblerDestroy(assembler);
			assembler = NULL;
		}
	}

	return assembler;
}


static int add_data_symbol(Assembler* assembler, const char* label)
{
	Symbol symbol = {assembler->data_counter, 'd'};
	if (HASH_MAP_SUCCESS != hashMapInsert(assembler->sym_table, label, &symbol))
	{
		return -1;
	}
	return 0;
}


static int parse_string_unit(Assembler* assembler, const char* line, const char* label)
{
	if (label)
	{
		if (0 != add_data_symbol(assembler, label))
		{
			return -1;
		}
		debug("Successfully added new label");
	}

	char* error_msg = NULL;
	char* string = parse_string(line, &error_msg);
	
	if (string)
	{
		debug("Parsed string: %s", string);
	}	

	LinkedList* string_list = linkedListInit(str_copy, str_cmp, str_free);
	linkedListInsert(string_list, string);
	hashMapInsert(assembler->data_table, &assembler->data_counter, string_list);
	assembler->data_counter += strlen(string) + 1;
	free(string);
	return 0;
}


static int parse_data_unit(Assembler* assembler, const char* line, const char* label)
{
	if (label)
	{
		if (0 != add_data_symbol(assembler, label))
		{
			//error("Failed to add symbol to the symbol table");
			return -1;
		}
		debug("Successfully added new label");
	}

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

	debug("Inserting data elemets to the data table");
	hashMapInsert(assembler->data_table, &assembler->data_counter, data_list);
	debug("Elements successfully inserted");
	assembler->data_counter += list_size;
}


static void /* todo: change retval */ parseLine(Assembler* assembler, FileReader* fr, char* line)
{
	debug("Parsing line: %s", line);
	char* label = NULL;
	char* itr = search_for_label(line, &label);
	
	if (label)
	{
		debug("Found label: %s", label);
	}
	
	if (is_data(line))
	{
		debug("Data directive detected");
		parse_data_unit(assembler, line, label);
	}
	else if (is_string(line))
	{
		debug("String directive detected");
		parse_string_unit(assembler, line, label);
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
		hashMapDestroy(assembler->data_table);
		hashMapDestroy(assembler->sym_table);
		hashMapDestroy(assembler->ext_table);
		hashMapDestroy(assembler->ent_table);
		free(assembler);
	}
}

