#ifndef __PARSER_H__
#define __PARSER_H__

#include <stddef.h>	// size_t
#include "linked_list.h"


const char* skip_whitespace(const char* expr);
const char* skip_directive(const char* expr);
const char* search_for_label(const char* line, char** label);
int parse_int(const char* expr, char** error_msg);
char* parse_string(const char* expr, char** error_msg);
LinkedList* parse_data(const char* expr, char** error_msg);

#endif // __PARSER_H__
