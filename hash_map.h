#ifndef __HASH_MAP_H__
#define __HASH_MAP_H__

#include <stddef.h>	// size_t

typedef struct hash_map HashMap;

typedef size_t (*key_hash_func_t)(const void*, size_t size);
typedef void* (*key_copy_func_t)(const void*);
typedef int (*key_cmp_func_t)(const void*, const void*);
typedef void (*key_free_func_t)(void*);

typedef void* (*value_copy_func_t)(const void*);
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
} HashMapStatus;


HashMap* hashMapInit(key_hash_func_t key_hash_func,
		     key_cmp_func_t key_cmp_func,
	       	     HashMapEntryHandlers handlers);

void hashMapClear(HashMap* map);
void hashMapDestroy(HashMap* map);

HashMapStatus hashMapInsert(HashMap* map, const void* key, const void* value);
int hashMapContains(const HashMap* map, const void* key);
void* hashMapGet(HashMap* map, const void* key);
void hashMapRemove(HashMap* map, const void* key);

size_t hashMapSize(const HashMap* map);

typedef void (*for_each_func_t) (void* data, void* params);

void hashMapForEach(HashMap* map, for_each_func_t, void* params);


#endif // __HASH_MAP_H__
