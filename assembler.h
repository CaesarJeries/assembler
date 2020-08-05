#ifndef __ASSEMBLY_H__
#define __ASSEMBLY_H__

int is_comment(const char* expr);
int is_symbol(const char* expr);
int is_extern(const char* expr);
int is_entry(const char* expr);
int is_data(const char* expr);
int is_string(const char* expr);


#endif // __ASSEMBLY_H__
