#ifndef __HASH_MAP_H__
#define __HASH_MAP_H__

#include <stddef.h>	// size_t

typedef struct hash_map HashMap;

typedef size_t (*key_hash_func_t)(void*, size_t size);
typedef void* (*key_copy_func_t)(void*);
typedef int (*key_cmp_func_t)(void*, void*);
typedef void (*key_free_func_t)(void*);

typedef void* (*value_copy_func_t)(void*);
typedef void (*value_free_func_t)(void*);

typedef struct
{
	key_copy_func_t key_copy;
	key_free_func_t key_free;
	value_copy_func_t value_copy;
	value_free_func_t value_free;
} HashMapEntryHandlers;



typedef enum
{
	HASH_MAP_SUCCESS,
	HASH_MAP_MEM_ERROR
} HashMapErrorCode;



HashMap* hashMapInit(key_hash_func_t key_hash_func,
		     key_cmp_func_t key_cmp_func,
	       	     HashMapEntryHandlers handlers);


int hashMapContains(const HashMap* map, void* key);

int hashMapInsert(HashMap* map, void* key, void* value);
int hashMapUpdate(HashMap* map, void* key, void* value);

void* hashMapGet(HashMap* map, void* key);
void hashMapRemove(HashMap* map, void* key);

size_t hashMapSize(const HashMap* map);
int hashMapEmpty(const HashMap* map);

void hashMapDestroy(HashMap* map);


#endif // __HASH_MAP_H__
