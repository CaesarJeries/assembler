#include <assert.h>
#include <ctype.h>	// isdigit, isspace
#include <malloc.h>

#include "linked_list.h"
#include "logging.h"
#include "string.h"
#include "parser.h"


static char* skip_whitespace(char* itr)
{
	while (*itr && isspace(*itr))
	{
		++itr;
	}
	
	return itr;
}


char* search_for_label(char* line, char** label_dst)
{
	assert(line);
	assert(label_dst);

	char* itr = line;
	char* start = NULL;
	char* end = NULL;
	
	start = skip_whitespace(itr);
	end = start;	
	while (*end)
	{
		if (':' == *end)
		{
			*end = 0;
			char* label = strdup(start);
			*end = ':';
			*label_dst = label;
			return end + 1;
		}
		++end;
	}
	
	*label_dst = NULL;
	return line;
}


int parse_int(const char* expr, char** error_msg)
{
	debug("Parsing int from %s", expr);
	const char* itr = expr;
	char sign = '*'; // initialize with a sentinel value

	while (*itr && isspace(*itr)) ++itr;

	if (*itr == '-' || *itr == '+')
	{
		sign = *itr;
		++itr;
	}

	const char* str_start = itr;
	while (*itr && '\n' != *itr)
	{
		if  (isspace(*itr))
		{
			++itr;
			continue;
		}
		if (!isdigit(*itr))
		{
			debug("Invalid character: %c", *itr);
			*error_msg = "Encountered a non-digit character while parsing an integer";
			return 0;
		}
		
		++itr;
	}
	const char* str_end = itr;
	size_t str_length = str_end - str_start;
	if (0 == str_length)
	{
		*error_msg = "Expected a number but got empty string";
		return 0;
	}

	int exp = 1;
	int number = 0;
	for (itr = str_start; itr < str_end; ++itr)
	{
		int digit = *itr - '0';
		if (0 == digit && 1 == exp) continue;
		number *= exp;
		number += digit;
		exp *= 10;
	}

	if (sign == '-') number *= -1;

	return number;
}


char* parse_string(const char* expr, char** error)
{
	const char* itr = expr;
	if ('"' != *itr)
	{
		*error = "A string must be enclosed with \"";
		return NULL;
	}

	++itr;
	const char* str_start = itr;

	while ('"' != *itr)
	{
		if ('\n' == *itr)
		{
			*error = "New line encountered in the middle of a string";
			return NULL;
		}
		
		++itr;
	}

	const char* str_end = itr;
	size_t str_length = str_end - str_start;
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



static void* str_copy(void* str)
{
	return strdup(str);
}

static int str_compare(void* s1, void* s2)
{
	return strcmp(s1, s2);
}

static void str_free(void* s)
{
	free(s);
}


#define MAX_DIGIT_COUNT 42

static const char* skip_directive(const char* expr)
{
	const char* itr = expr;
	int directive_found = 0;
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

	return itr;
}
		


LinkedList* parse_data(const char* expr, char** error)
{
	LinkedList* list = linkedListInit(str_copy, str_compare, str_free);
	char* copy = strdup(skip_directive(expr));
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
			debug("Successfully parsed value: %d", value);
			itoa(int_repr, value);
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

