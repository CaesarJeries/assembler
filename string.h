#ifndef __MY_STRING_H__
#define __MY_STRING_H__

#include <stddef.h>
#include <string.h>


/**
 * Converts n to binary.
 * 'dst' must be at least WORD_SIZE + 1.
 **/
void int_to_bin(int n, char* dst);

/**
 * Converts the binary representaion stored in 'dst'
 * to decimal.
 * If dst is not a valid string, or if it contains non-digit characters,
 * other than '+'/'-', then the behaviour of this function is undefined.
 **/
int bin_to_int(const char* dst);

/**
 * Duplicates the given string.
 * If memory allocation fails, NULL is returned.
 *
 * If 'str' is not a valid C-string, behaviour is undefined.
 **/
char* strdup(const char* str);


/**
 * Duplicates the first 'size' characters of given string.
 * If memory allocation fails, NULL is returned.
 *
 * If 'str' is not a valid C-string, behaviour is undefined.
 **/
char* strndup(const char* str, size_t size);


/**
 * Converts 'number' to string, with sign handling.
 * 'dst' must contain enough space to store the representation
 * of 'number'.
 **/
void itoa(char* dst, int number);

/**
 * Returns a dynamically allocated string that is the concatenation
 * of 's1' and 's2'.
 * In case of a memory allocation error, NULL is returned.
 **/
char* strappend(const char* s1, const char* s2);

#endif // __MY_STRING_H__
