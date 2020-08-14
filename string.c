#include <malloc.h>

#include "string.h"

#define AUX_BUFF_SIZE 32

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



void int_to_bin(int n, char* dst)
{
	static char aux[AUX_BUFF_SIZE] = {0};
	memset(aux, 0, AUX_BUFF_SIZE);
	
	if (0 == n)
	{
		dst[0] = '0';
		dst[1] = 0;
		return;
	}

	char* itr = aux;
	while (n)
	{
		*itr = '0' + n % 2;
		n >>= 1;
		++itr;
	}

	*itr = 0;
	reverse(aux);
	strncpy(dst, aux, strlen(aux));
}


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


char* strndup(const char* str, size_t size)
{
	char* new_str = malloc(size + 1);
	if (new_str)
	{
		strncpy(new_str, str, size);
	}
	
	new_str[size] = 0;
	return new_str;

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

void itoa(char* dst, int orig)
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
	char* itr = dst;
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
	reverse(dst);
}

char* strappend(const char* s1, const char* s2)
{
	size_t size1 = strlen(s1);
	size_t size2 = strlen(s2);
	size_t total_size = size1 + size2 + 1;
	char* str = malloc(total_size);
	if (str)
	{
		strcpy(str, s1);
		strcpy(str + size1, s2);
		str[total_size - 1] = 0;
	}
	
	return str;
}
