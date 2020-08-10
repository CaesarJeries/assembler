#ifndef __GRAMMAR_H__
#define __GRAMMAR_H__

int is_comment(const char* expr);
int is_whitespace(const char* expr);
int is_symbol(const char* expr);
int is_extern(const char* expr);
int is_entry(const char* expr);
int is_data(const char* expr);
int is_string(const char* expr);


#endif // __GRAMMAR_H__
