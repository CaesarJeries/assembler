#include <malloc.h>

#include "common.h"
#include "logging.h"
#include "string.h"

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


static void bitwise_not(char* str)
{
	static char aux[WORD_SIZE + 1] = {0};
	memset(aux, '1', WORD_SIZE);
	aux[WORD_SIZE] = 0;
	
	size_t str_len = strlen(str);	
	for (size_t i = 0; i < str_len; ++i)
	{
		// bitwise not
		str[i] = '0' + ('1' - str[i]);
	}
	
	debug("Moving %s of size %lu to index %lu", str, str_len, WORD_SIZE - str_len);
	memmove(aux + WORD_SIZE - str_len, str, str_len);
	debug("After memmove: %s", aux);
	strcpy(str, aux);
}

static void complement(char* str)
{
	bitwise_not(str);

	size_t str_len = strlen(str);
	debug("Adding 1 after bitwise not. strlen = %lu", str_len);
	int carry = 1;
	for (int i = str_len - 1; i >= 0; --i)
	{
		char new_value = str[i] + carry;
		debug("new value: %c", new_value);

		if ('2' == new_value)
		{
			carry = 1;
			new_value = '0';
		}
		else
		{
			carry = 0;
		}
		
		debug("new value: %c", new_value);
		str[i] = new_value;
	}

	debug("After 2's complement: %s", str);
}

void int_to_bin(int n, char* dst)
{
	static char aux[WORD_SIZE + 1] = {0};
	memset(aux, 0, WORD_SIZE + 1);
	
	if (0 == n)
	{
		dst[0] = '0';
		dst[1] = 0;
		return;
	}

	int orig = n;
	if (n < 0) n *= -1;

	char* itr = aux;
	while (n)
	{
		*itr = '0' + n % 2;
		n >>= 1;
		++itr;
	}

	*itr = 0;
	reverse(aux);

	if (orig < 0)
	{
		debug("Before complement: %s", aux);
		complement(aux);
		debug("After complement: %s", aux);
	}

	strncpy(dst, aux, strlen(aux));
}


static int get_value(const char* a)
{
	int num = 0;
	do {
		int b = (*a=='1');
		num = (num<<1) | b;
		a++;

	} while (*a);

	return num;
}


int bin_to_int(const char* str)
{
	debug("Converting binary to int: %s", str);
	if (str[0] == '1')
	{
		char* copy = strdup(str);
		complement(copy);
		int value = get_value(copy);
		free(copy);

		debug("Value: %d", -value);
		return -value;
	}

	int value = get_value(str);
	debug("Value: %d", value);
	return value;
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
