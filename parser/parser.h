#ifndef __PARSER_H__
#define __PARSER_H__

int parse_int(const char* expr, char** error_msg);
char* parse_string(const char* expr, char** error_msg);


#endif // __PARSER_H__
