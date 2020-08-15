#ifndef __PARSER_H__
#define __PARSER_H__

#include <stddef.h>	// size_t
#include "linked_list.h"

/**
 * Parses a string located between the addresses [start, end),
 * and returns its decimal value.
 **/
int str_to_int(const char* start, const char* end);

/**
 * Returns the address of the first non-whitespace character in 'expr'.
 * If 'expr' contains only whitespaces, NULL is returned.
 **/
const char* skip_whitespace(const char* expr);

/**
 * expr must be a directive: .string, .data, .extern, .entry.
 * This function returns the address of the first character after the
 * string of the directive.
 **/
const char* skip_directive(const char* expr);


/**
 * Searches for a label in the given line.
 * If a label is found, its address is stored in the out parameter 'label',
 * and the address of the rest of the line is returned (after the label).
 * Otherwise, 'label' is set to NULL, and the address of 'line' is returned.
 **/
const char* search_for_label(const char* line, char** label);

/**
 * Parses an integer from the given expression.
 * On success, 0 is returned.
 * Otherwise, -1 is returned, and 'error_msg' is updated with the corresponding error message.
 **/
int parse_int(const char* expr, char** error_msg);

/**
 * Parses a string from the given expression.
 * On success, a duplicate of the string is returned. This value is dynamically allocated..
 * Otherwise, NULL is returned, and 'error_msg' is updated with the corresponding error message.
 **/
char* parse_string(const char* expr, char** error_msg);

/**
 * Parses a data array from the given expression.
 * On success, this function returns a linked list containing the binary represenation
 * of each element, in string form.
 * Othersise, NULL is returned, and 'error_msg' is updated to contain the corresponding error message.
 **/
LinkedList* parse_data(const char* expr, char** error_msg);

#endif // __PARSER_H__
