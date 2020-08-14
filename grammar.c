#include <ctype.h>	// isspace

#include "command.h"
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

int is_keyword(const char* label)
{
	static char* keywords[] = {
		"entry",
		"extern",
		"data",
		"string"};

	for (size_t i = 0; i < sizeof(keywords)/sizeof(keywords[0]); ++i)
	{
		if (0 == strcmp(label, keywords[i]))
		{
			return 1;
		}
	}

	if (is_register(label)) return 1;
	if (is_command(label)) return 1;

	return 0;
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
