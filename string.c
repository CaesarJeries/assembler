#include <malloc.h>

#include "string.h"

char* strdup(const char* str)
{
	size_t len = strlen(str);
	char* new_str = malloc(len + 1);
	if (new_str)
	{
		strcpy(new_str, str);
	}
	
	new_str[len] = 0;
	return new_str;
}


static void reverse(char* str)
{
	size_t length = strlen(str);
	char* left = str;
	char* right = str + length - 1;
	while (left < right)
	{
		char temp = *left;
		*left = *right;
		*right = temp;
		++left;
		--right;
	}
}

static int num_digits(int n)
{
	if (0 == n) return 1;
	if (n < 0) return num_digits(-1 * n);
	
	int length = 0;
	while (n)
	{
		++length;
		n /= 10;
	}
	return length;
}

char* itoa(int orig)
{
	int number = orig;
	size_t length = 0;
	int is_signed = 0;
	if (orig < 0)
	{
		is_signed = 1;
		++length;
		number *= -1;
	}

	int ndigits = num_digits(orig);
	length += ndigits;
	char* retval = malloc(length + 1);
	if (retval)
	{
		char* itr = retval;
		while (ndigits)
		{
			char c = '0' + number % 10;
			number /= 10;
			--ndigits;
			*itr = c;
			++itr;
		}

		if (is_signed) *itr++ = '-';
		
		*itr = 0;
		reverse(retval);
	}

	return retval;
}

