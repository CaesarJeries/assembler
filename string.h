#ifndef __MY_STRING_H__
#define __MY_STRING_H__

#include <stddef.h>
#include <string.h>

void int_to_bin(int n, char* dst);
char* strdup(const char* str);
char* strndup(const char* str, size_t size);
void itoa(char* dst, int number);

char* strappend(const char* s1, const char* s2);

#endif // __MY_STRING_H__
