#include <ctype.h>	// isspace

#include "string.h"
#include "grammar.h"

int is_comment(const char* expr)
{
	return NULL != strchr(expr, ';');
}

int is_whitespace(const char* expr)
{
	while (*expr)
	{
		if (!isspace(*expr))
			return 0;
		++expr;
	}
	return 1;
}


int is_symbol(const char* expr)
{
	return 0; // todo: implement
}

int is_extern(const char* expr)
{
	return NULL != strstr(expr, ".extern");
}

int is_entry(const char* expr)
{
	return NULL != strstr(expr, ".entry");
}


int is_data(const char* expr)
{
	return NULL != strstr(expr, ".data");
}


int is_string(const char* expr)
{
	return NULL != strstr(expr, ".string");
}
