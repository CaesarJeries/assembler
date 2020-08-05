#include <string.h>
#include <malloc.h>

char* strdup(const char* str)
{
	size_t len = strlen(str);
	char* new_str = malloc(len);
	if (new_str)
	{
		strcpy(new_str, str);
	}

	return new_str;
}

