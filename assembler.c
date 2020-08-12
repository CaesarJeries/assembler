#include <assert.h>
#include <errno.h>
#include <malloc.h>
#include <stddef.h>	// size_t

#include "assembler.h"
#include "command.h"
#include "file_reader.h"
#include "grammar.h"
#include "hash_map.h"
#include "linked_list.h"
#include "logging.h"
#include "parser.h"
#include "string.h"

#define CODE_SEGMENT_START_ADDR 100
#define MAX_FILENAME_LENGTH 128
#define WORD_SIZE 24
#define MAX_LABEL_LENGTH 128

static AssemblerStatus status = ASSEMBLER_SUCCESS;

typedef int (*line_parser_t)(Assembler*, FileReader*, const char*);


typedef struct
{
	size_t value;
	char type;

} Symbol;


typedef enum
{
	DATA_SYMBOL = 'd',
	EXT_SYMBOL = 'e',
	CODE_SYMBOL = 'c',
	ENTRY_SYMBOL = 'm'

} SymbolType;


struct assembler
{
	size_t data_counter;
	size_t inst_counter;

	HashMap* data_table;	
	HashMap* code_table;
	HashMap* sym_table;
	HashMap* ext_table;
	LinkedList* ent_list;
};

typedef struct
{
	char* binary_code;
	size_t ic;
	size_t additional_words;
	char* label;
	AddressingMethod method;

} InstructionEntry;

void inst_entry_free(void* p)
{
	InstructionEntry* entry = p;
	if (entry)
	{
		free(entry->binary_code);
		free(entry->label);
		free(entry);
	}
}

void* inst_entry_copy(const void* other)
{
	InstructionEntry* new_entry = malloc(sizeof(*new_entry));
	const InstructionEntry* other_entry = other;
	if (new_entry)
	{
		new_entry->binary_code = strdup(other_entry->binary_code);
		if (other_entry->label)
		{
			new_entry->label = strdup(other_entry->label);
			if (!new_entry->label)
			{
				inst_entry_free(new_entry);
				return NULL;
			}
		}
		else
		{
			new_entry->label = NULL;
		}

		if (!new_entry->binary_code)
		{
			inst_entry_free(new_entry);
			return NULL;
		}

		new_entry->ic = other_entry->ic;
		new_entry->additional_words = other_entry->additional_words;
		new_entry->method = other_entry->method;
	}

	return new_entry;
}


static size_t str_hash(const void* ptr, size_t size)
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

static size_t int_hash(const void* n, size_t size)
{
	return (*(const size_t*) n) % size;
}

static int int_cmp(const void* a, const void* b)
{
	int first = *(const int*) a;
	int second = *(const int*) b;

	return first - second;
}


static void* int_copy(const void* n)
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


static void* str_copy(const void* s)
{
	return strdup(s);
}

static void str_free(void* s)
{
	free(s);
}

static int str_cmp(const void* s1, const void* s2)
{
	return strcmp(s1, s2);
}

static void* symbolCopy(const void* other)
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

static void* list_copy(const void* list)
{
	return linkedListCopy(list);
}

static void list_free(void* list)
{
	linkedListDestroy(list);
}

static char* get_empty_word(void)
{
	char* word = malloc(WORD_SIZE + 1);
	memset(word, '0', WORD_SIZE);
	word[WORD_SIZE] = 0;

	return word;
}

static int add_symbol(Assembler* assembler, const char* label, size_t value, SymbolType type)
{
	debug("Adding symbol: %s", label);
	Symbol symbol = {value, type};
	if (HASH_MAP_SUCCESS != hashMapInsert(assembler->sym_table, label, &symbol))
	{
		return -1;
	}
	debug("Symbol successfully added");
	return 0;
}

static int add_data_symbol(Assembler* assembler, const char* label)
{
	return add_symbol(assembler, label, assembler->data_counter, DATA_SYMBOL);

}

static int add_ext_symbol(Assembler* assembler, const char* label)
{
	return add_symbol(assembler, label, 0, EXT_SYMBOL);
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
			error("%s", "Failed to add symbol to the symbol table");
			return -1;
		}
		debug("Successfully added new label");
	}

	char* error_msg = NULL;	
	LinkedList* data_list = parse_data(line, &error_msg);
	if (!data_list)
	{
		error("%s", error_msg);
		return -1;
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
	return 0;
}

static char* get_label_from_directive(const char* line)
{
	const char* itr = skip_directive(line);
	static char label[MAX_LABEL_LENGTH] = {0};
	memset(label, 0, MAX_LABEL_LENGTH);
	
	int read_elements = sscanf(itr, " %s ", label);

	if (read_elements < 1)
	{
		error("No label was provided for the directive: %s", line);
		status = ASSEMBLER_PARSE_ERR;
		return NULL;
	}

	return strdup(label);
}

static int parse_extern_unit(Assembler* assembler, const char* line, const char* label)
{
	char* ext_label = get_label_from_directive(line);
	if (!ext_label) return -1;

	return add_ext_symbol(assembler, ext_label);
}



static char* get_command_obj(const char* command_name,
			     const char* src_op,
			     const char* dst_op)
{
	char* result = get_empty_word();
	if (result)
	{
		Command cmd_def = get_command_definition(command_name);

		int_to_bin(cmd_def.op_code, result + OP_CODE_OFFSET);
		int_to_bin(cmd_def.funct, result + FUNCT_OFFSET);
		result[A_OFFSET] = '1';

		if (cmd_def.has_src)
		{
			int_to_bin(get_addr_method(src_op), result + SRC_ADDR_OFFSET);
			if (is_register(src_op))
			{
				int_to_bin(get_register_number(src_op), result + SRC_REG_OFFSET);
			}
		}
		
		if (cmd_def.has_dst)
		{
			int_to_bin(get_addr_method(dst_op), result + DST_ADDR_OFFSET);
			if (is_register(dst_op))
			{
				int_to_bin(get_register_number(dst_op), result + DST_REG_OFFSET);
			}
		}
	}
	
	return result;
}

/*
void placeholder()
{
	char* label = get_label(operand); assert(label);
	debug("Searching for symbol");
	Symbol* symbol = hashMapGet(assembler->sym_table, label);
	if (!symbol)
	{
		error("Symbol not found: %s", label);
		status = ASSEMBLER_MISSING_SYM;
		return NULL;
	}
	debug("Symbol found. Converting value to binary");	
	int_to_bin(symbol->value, word);
	free(label);
}
*/

static void write_value(char* dst, size_t value)
{
	static char aux[WORD_SIZE + 1] = {0};
	memset(aux, 0, WORD_SIZE + 1);

	int_to_bin(value, aux);
	
	char* dst_itr = dst + A_OFFSET - 1;
	const char* src_itr = aux + strlen(aux) - 1;

	debug("Writing %s to the destination word", aux);	
	for (size_t i = 0; i < strlen(aux); ++i)
	{
		*dst_itr = *src_itr;
		--dst_itr;
		--src_itr;
	}
}

static char* resolve_word(Assembler* assembler, const char* operand)
{
	AddressingMethod method = get_addr_method(operand);
	debug("Addressing method for %s: %d", operand, method);

	if (REGISTER_ADDRESSING == method)
	{
		debug("No additional words required");
		return NULL;
	}
	
	char* word = get_empty_word();	
	if (!word)
	{
		status = ASSEMBLER_MEM_ERR;
		return NULL;
	}

	if (IMMEDIATE_ADDRESSING == method)
	{
		int value = get_value(operand);
		write_value(word, value);
		word[A_OFFSET] = '1';
	}
	
	return word;
}



static LinkedList* get_additional_words(Assembler* assembler,
					const char* command_name,
					const char* src_op,
					const char* dst_op)
{
	assert(command_name); assert(src_op); assert(dst_op);

	LinkedList* list = linkedListInit(str_copy, str_cmp, str_free);
	if (list)
	{
		Command cmd_def = get_command_definition(command_name);
		if (cmd_def.has_src)
		{
			debug("Resolving source operand");
			char* word = resolve_word(assembler, src_op);
			if (word)
			{
				debug("Additional word for the source operand: %s", word);
				linkedListInsert(list, word);
				debug("Word inserted to list");
				free(word);
			}
		}

		if (cmd_def.has_dst)
		{
			debug("Resolving destination operand");
			char* word = resolve_word(assembler, dst_op);
			if (word)
			{
				debug("Additional word for the destination operand: %s", word);
				linkedListInsert(list, word);
				debug("Word inserted to list");
				free(word);
			}
		}
	}
	
	return list;
}



static int add_inst_symbol(Assembler* assembler, const char* label)
{
	if (!label) return 0;
	return add_symbol(assembler, label, assembler->inst_counter, CODE_SYMBOL);
}

static int parse_command_unit(Assembler* assembler, const char* line, const char* label)
{
	static char cmd_name[MAX_CMD_SIZE] = {0};
	static char src_op[MAX_OP_SIZE] = {0};
	static char dst_op[MAX_OP_SIZE] = {0};

	memset(cmd_name, 0, MAX_CMD_SIZE);
	memset(src_op, 0, MAX_OP_SIZE);
	memset(dst_op, 0, MAX_OP_SIZE);

	if (0 != parse_command(line, cmd_name, src_op, dst_op))
	{
		error("Failed to parse instruction: %s", line);
		return -1;
	}

	if (add_inst_symbol(assembler, label)) return -1;
	char* command = get_command_obj(cmd_name, src_op, dst_op);
	LinkedList* list = get_additional_words(assembler, cmd_name, src_op, dst_op);
	if (!command || !list) return -1;

	size_t list_size = linkedListSize(list);
	debug("Command binary: %s", command);
	InstructionEntry entry = {0};
	entry.binary_code = command;
	entry.ic = assembler->inst_counter;
	entry.additional_words = list_size;
	size_t location = CODE_SEGMENT_START_ADDR + assembler->inst_counter;
	debug("Adding word to instruction table: %s. Label: %s", entry.binary_code, entry.label);
	hashMapInsert(assembler->code_table, &location, &entry);
	free(command); command = NULL;
	++assembler->inst_counter;
	
	debug("Instruction requires %lu additional words", list_size);
	for(size_t i = 0; i < list_size; ++i)
	{
		const char* operand = src_op;
		char* operand_label = NULL;
		if (1 == list_size || 1lu == i)
		{
			operand = dst_op;
		}
		
		operand_label = get_label(operand);
		debug("Operand label: %s", operand_label);

		char* word = linkedListGetAt(list, i);
		entry.binary_code = word;
		entry.method = get_addr_method(operand);
		entry.label = operand_label;
		entry.ic = assembler->inst_counter;
		entry.additional_words = 0;
		location = CODE_SEGMENT_START_ADDR + assembler->inst_counter;
		debug("Adding word to instruction table: %s. Label: %s", word, entry.label);
		hashMapInsert(assembler->code_table, &location, &entry);
		++assembler->inst_counter;
	}
	
	debug("Final instruction count: %lu", assembler->inst_counter);	
	debug("Final data count: %lu", assembler->data_counter);	
	linkedListDestroy(list);
	return 0;
}


Assembler* assemblerInit()
{
	Assembler* assembler = malloc(sizeof(*assembler));
	if (assembler)
	{
		assembler->data_counter = 0;
		assembler->inst_counter = CODE_SEGMENT_START_ADDR;
		
		HashMapEntryHandlers data_handlers = {int_copy, int_free,
						     list_copy, list_free};

		assembler->data_table = hashMapInit(int_hash, int_cmp, data_handlers);
		
		HashMapEntryHandlers inst_handlers = {int_copy, int_free,
						      inst_entry_copy, inst_entry_free};
		assembler->code_table = hashMapInit(int_hash, int_cmp, inst_handlers);

		HashMapEntryHandlers symbol_handlers = {str_copy, str_free,
							symbolCopy, symbolFree};
		assembler->sym_table = hashMapInit(str_hash, str_cmp, symbol_handlers);
		
		HashMapEntryHandlers handlers = {str_copy, str_free,
						 str_copy, str_free};
		assembler->ext_table = hashMapInit(str_hash, str_cmp, handlers);
		assembler->ent_list = linkedListInit(str_copy, str_cmp, str_free);
		
		if (!assembler->code_table ||
		    !assembler->data_table ||
		    !assembler->sym_table ||
		    !assembler->ext_table ||
		    !assembler->ent_list)
		{
			error("%s", "Failed to initialize assembler object");
			assemblerDestroy(assembler);
			assembler = NULL;
		}
	}

	return assembler;
}



static int parse_line_first_pass(Assembler* assembler, FileReader* fr, const char* line)
{
	debug("Parsing line: %s", line);
	char* label = NULL;
	const char* itr = search_for_label(line, &label);
	int parse_status = 0;	
	if (label)
	{
		debug("Found label: %s", label);
	}

	if (is_entry(line)) return 0;
	
	if (is_data(itr))
	{
		debug("Data directive detected");
		parse_status = parse_data_unit(assembler, itr, label);
	}
	else if (is_string(itr))
	{
		debug("String directive detected");
		parse_status = parse_string_unit(assembler, itr, label);
	}
	else if (is_extern(itr))
	{
		debug("Extern directive detected");
		parse_status = parse_extern_unit(assembler, itr, label);
	}
	else
	{
		debug("Trying to parse instruction");
		parse_status = parse_command_unit(assembler, itr, label);
	}
	
	if (0 != parse_status)
	{
		error("Error encountered at line: %lu", fileReaderGetLineNum(fr));
	}
	
	free(label);
	return status;
}

static void write_data_word(Assembler* assembler, InstructionEntry* entry, size_t value)
{
	char* dst = entry->binary_code;
	AddressingMethod method = entry->method;
	
	write_value(dst, value);
	debug("Written value to data word: %s", dst);
	
	if (RELATIVE_ADDRESSING == method)
	{
		dst[A_OFFSET] = '1';
	}
	else if (DIRECT_ADDRESSING == method)
	{
		Symbol* symbol = hashMapGet(assembler->sym_table, entry->label);
		assert(symbol);

		if (EXT_SYMBOL == symbol->type)
		{
			dst[E_OFFSET] = '1';
		}
		else
		{
			dst[R_OFFSET] = '1';
		}
	}
}

static int update_code_table(Assembler* assembler)
{
	debug("Updating instruction table");
	size_t end = CODE_SEGMENT_START_ADDR + assembler->inst_counter;
	for (size_t i = CODE_SEGMENT_START_ADDR; i < end; ++i)
	{
		InstructionEntry* entry = hashMapGet(assembler->code_table, &i);
		assert(entry);

		if (!entry->label) continue;

		debug("Looking for symbol: %s", entry->label);
		Symbol* symbol = hashMapGet(assembler->sym_table, entry->label);
		if (!symbol)
		{
			error("Symbol not found: %s", entry->label);
			return -1;
		}
		
		write_data_word(assembler, entry, symbol->value);
		debug("Updated word: %s", entry->binary_code);
	}

	return 0;
}


static int parse_line_second_pass(Assembler* assembler, FileReader* fr, const char* line)
{
	if (is_entry(line))
	{
		debug("Handling entry directive: %s", line);
		char* label = get_label_from_directive(line);
		if (!label) return -1;
		
		debug("Adding symbol to the entry list: %s", label);	
		linkedListInsert(assembler->ent_list, label);	

		debug("Looking up symbol: %s", label);
		Symbol* symbol = hashMapGet(assembler->sym_table, label);
		if (!symbol)
		{
			error("Symbol not found: %s", label);
			status = ASSEMBLER_MISSING_SYM;
			return -1;
		}

		symbol->type = ENTRY_SYMBOL;
		free(label);
	}

	return 0;
}

static int pass(Assembler* assembler, FileReader* fr, line_parser_t parse_line)
{
	assert(assembler);
	assert(fr);

	debug("Starting pass");
	FileReaderStatus read_status = FILE_READER_SUCCESS;
	while (1)
	{
		static char line[MAX_LINE_SIZE + 1] = {0};
		read_status = fileReaderGetLine(fr, line);
		debug("Read line. Checking status.");
		if (FILE_READER_SUCCESS != read_status)
		{
			debug("File reader failed to read line");
			if (FILE_READER_EOF == read_status)
			{
				debug("Reached end of file");
				return ASSEMBLER_SUCCESS;
			}

			const char* filename = fileReaderGetFilename(fr);
			error("Failed to read from file: %s: %s\n", filename, strerror(errno));
			fileReaderDestroy(fr);
			return ASSEMBLER_FILE_READ_ERR;
		}
		parse_line(assembler, fr, line);
	}

	return 0;
}

static int firstPass(Assembler* assembler, FileReader* fr)
{
	return pass(assembler, fr, parse_line_first_pass);
}

static int secondPass(Assembler* assembler, FileReader* fr)
{
	int result = pass(assembler, fr, parse_line_second_pass);
	if (0 != result) return -1;
	
	return update_code_table(assembler);
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

static void output_entry_file(Assembler* assembler, const char* basename)
{
	size_t num_entries = linkedListSize(assembler->ent_list);
	if (0 == num_entries) return;

	char* filename = strappend(basename, ".ent");
	FILE* file = fopen(filename, "w+");
	if (!file)
	{
		error("Failed to open file: %s", filename);
		return;
	}

	for (size_t i = 0; i < num_entries; ++i)
	{
		char* label = linkedListGetAt(assembler->ent_list, i);
		assert(label);

		Symbol* symbol = hashMapGet(assembler->sym_table, label);
		assert(symbol);

		fprintf(file, "%s %7lu\n", label, symbol->value);
	}

	free(filename);
}


static void output_files(Assembler* assembler, const char* basename)
{
	output_entry_file(assembler, basename);
	//output_binary_file(assembler, basename);
	//output_extern_file(assembler, basename);
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
		status = secondPass(assembler, fr);
		if (ASSEMBLER_SUCCESS == status)
		{
			debug("Both passed completed successfully. Writing output files");
			output_files(assembler, fileReaderGetBasename(fr));
		}

		fileReaderDestroy(fr);
		return status;
	}
	
	fileReaderDestroy(fr);
	return ASSEMBLER_PARSE_ERR;
}


void assemblerReset(Assembler* assembler)
{
	assert(assembler);
	
	assembler->data_counter = 0;
	assembler->inst_counter = 0;

	hashMapClear(assembler->code_table);
	hashMapClear(assembler->data_table);
	hashMapClear(assembler->sym_table);
	hashMapClear(assembler->ext_table);
	linkedListClear(assembler->ent_list);
}


void assemblerDestroy(Assembler* assembler)
{
	if (assembler)
	{
		debug("Destroying assembler object");
		hashMapDestroy(assembler->code_table);
		hashMapDestroy(assembler->data_table);
		hashMapDestroy(assembler->sym_table);
		hashMapDestroy(assembler->ext_table);
		linkedListDestroy(assembler->ent_list);
		free(assembler);
	}
}

