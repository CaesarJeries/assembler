#include <string.h>
#include "assembly.h"

int is_comment(const char* expr)
{
	return NULL != strchr(expr, ';');
}


int is_symbol(const char* expr)
{
	// todo
}

int is_extern(const char* expr)
{
	return strstr(expr, ".extern");
}

int is_entry(const char* expr)
{
	return strstr(expr, ".entry");
}


int is_data(const char* expr)
{
	return strstr(expr, ".data");
}


int is_string(const char* expr)
{
	return strstr(expr, ".string");
}
