#include <assert.h>
#include <ctype.h>	// isdigit, isspace
#include <malloc.h>

#include "linked_list.h"
#include "logging.h"
#include "string.h"
#include "parser.h"


#define MAX_DIGIT_COUNT 42


const char* skip_whitespace(const char* expr)
{
	assert(expr);

	const char* itr = expr;
	while (*itr && isspace(*itr))
	{
		++itr;
	}
		
	return itr;
}


const char* skip_directive(const char* expr)
{
	const char* itr = expr;
	int directive_found = 0;
	
	itr = skip_whitespace(itr);
	while (*itr)
	{
		if (isspace(*itr) && directive_found)
		{
			return itr;
		}

		if ('.' == *itr)
		{
			directive_found = 1;
		}

		++itr;
	}

	if (!*itr) return expr;
	return itr;
}


const char* search_for_label(const char* line, char** label_dst)
{
	assert(line);
	assert(label_dst);

	const char* itr = line;
	const char* start = NULL;
	const char* end = NULL;
	
	start = skip_whitespace(itr);
	end = start;	
	while (*end)
	{
		if (':' == *end)
		{
			char* label = strndup(start, end - start);
			*label_dst = label;
			return end + 1;
		}
		++end;
	}
	
	return line;
}


int str_to_int(const char* str_start, const char* str_end)
{
	int exp = 1;
	int number = 0;
	for (const char* itr = str_start; itr < str_end; ++itr)
	{
		debug("Handling character: %c", *itr);
		int digit = *itr - '0';
		if (0 == digit && 1 == exp) continue;
		number *= exp;
		number += digit;
		exp = 10;
	}

	debug("Parsed %d from string", number);
	return number;
}


int parse_int(const char* expr, char** error_msg)
{
	const char* itr = skip_whitespace(expr);
	debug("Parsing int from %s", itr);
	char sign = '*'; // initialize with a sentinel value

	if ('-' == *itr || '+' == *itr)
	{
		sign = *itr;
		debug("Sign found: %c", sign);
		++itr;
	}

	const char* str_start = itr;
	debug("Parsing absolute value from %s", str_start);

	while ((*itr) && isdigit(*itr))
	{
		++itr;
	}

	if (!*itr)
	{
		if (isdigit(*itr) && !isspace(*itr))
		{
			*error_msg = "Encountered a non-digit character when parsing an integer";
			debug("Invalid character: %d", *itr);

			return -1;
		}
	}

	const char* str_end = itr;
	size_t str_length = str_end - str_start;
	if (0 == str_length)
	{
		*error_msg = "Expected a number but got empty string";
		return 0;
	}

	debug("number of digits: %d", str_length);
	int number = str_to_int(str_start, str_end);
	if (sign == '-') number *= -1;

	return number;
}


char* parse_string(const char* expr, char** error)
{
	debug("Parsing string from expression: %s", expr);
	const char* itr = expr;
	
	itr = skip_whitespace(skip_directive(itr));
	debug("Removed leading whitespace: %s", itr);
	if ('"' != *itr)
	{
		debug("Strings must start with \"");
		*error = "A string must be enclosed with \"";
		return NULL;
	}

	++itr;
	const char* str_start = itr;

	while ('"' != *itr)
	{
		if ('\n' == *itr)
		{
			debug("Encountered new line in the middle of a string");
			*error = "New line encountered in the middle of a string";
			return NULL;
		}
		
		++itr;
	}

	const char* str_end = itr;
	size_t str_length = str_end - str_start;
	debug("String length: %lu", str_length);
	char* new_str = malloc(str_length + 1);
	if (!new_str)
	{
		*error = "Failed to allocate memory while parsing string";
		return NULL;
	}
	
	char* new_itr = new_str;
	for (itr = str_start; itr < str_end; ++itr, ++new_itr)
	{
		*new_itr = *itr;
	}

	*new_itr = 0;
	return new_str;
}



static void* str_copy(const void* str)
{
	return strdup(str);
}

static int str_compare(const void* s1, const void* s2)
{
	return strcmp(s1, s2);
}

static void str_free(void* s)
{
	free(s);
}



LinkedList* parse_data(const char* expr, char** error)
{
	LinkedList* list = linkedListInit(str_copy, str_compare, str_free);
	const char* itr = skip_whitespace(expr);
	char* copy = strdup(skip_directive(itr));
	if (!copy || !list)
	{
		*error = "Failed to allocate memory while parsing data array";
		free(copy);
		linkedListDestroy(list);
		return NULL;
	}

	char* token = strtok(copy, ",");
	while (NULL != token)
	{
		debug("Handling token: %s", token);
		static char int_repr[MAX_DIGIT_COUNT] = {0};
		int value = parse_int(token, error);
		if (*error != NULL)
		{
			free(copy);
			linkedListDestroy(list);
			return NULL;
		}
		else
		{
			itoa(int_repr, value);
			debug("Successfully parsed value: %d", value);
			if (LINKED_LIST_SUCCESS != linkedListInsert(list, int_repr))
			{
				*error = "Failed to allocate memory while parsing data array";
				free(copy);
				linkedListDestroy(list);
				return NULL;
			}
			
			debug("Successfully added %s to the list", int_repr);
		}

		token = strtok(NULL, ",");
	}

	free(copy);
	return list;
}

