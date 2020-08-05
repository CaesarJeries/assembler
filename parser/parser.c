#include <ctype.h>	// isdigit

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
	while (*itr)
	{
		if (*itr == '\n') break;
		if (!isdigit(*itr))
		{
			*error_msg = "Encountered a non-digit character while parsing an integer";
			return -1;
		}
		
		++itr;
	}
	const char* str_end = itr;
	size_t str_length = str_end - str_start;
	if (0 == str_length)
	{
		*error_msg = "Expected a number but got empty string";
		return -1;
	}

	int exp = 1;
	int number = 0;
	for (itr = str_start; itr < str_end; ++itr)
	{
		number *= exp;
		int digit = *itr - '0';
		number += digit;
		exp *= 10;
	}

	if (sign == '-') number *= -1;

	return number;
}

