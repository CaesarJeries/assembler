#include <ctype.h>	// isdigit
#include <malloc.h>
#include <stdlib.h>	// itoa

#include "linked_list.h"
#include "string.h"
#include "parser.h"

int parse_int(const char* expr, char** error_msg)
{
	const char* itr = expr;
	char sign = '*'; // initialize with a sentinel value

	if (*expr == '-' || *expr == '+')
	{
		sign = *expr;
		++itr;
	}

	const char* str_start = itr;
	while ('\n' != *itr)
	{
		if (!isdigit(*itr))
		{
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



LinkedList* parse_data(const char* expr, char** error)
{
	LinkedList* list = linkedListInit(str_copy, str_compare, str_free);
	char* copy = strdup(expr);
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
		const int max_digit_count = 42;
		static char [max_digit_count] int_repr = {0};
		int value = parse_int(token, error);
		if (*error != NULL)
		{
			free(copy);
			linkedListDestroy(list);
			return NULL;
		}
		else
		{
			itoa(value, int_repr, /*base=*/10);
			if (LINKED_LIST_SUCCESS != linkedListInsert(list, int_repr))
			{
				*error = "Failed to allocate memory while parsing data array";
				free(copy);
				linkedListDestroy(list);
				return NULL;
			}
		}

		token = strtok(copy, ",");
	}

	free(copy);
	return list;
}

