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


/**
 * Initializes an empty hash table.
 * In case of a memory allocation error, NULL is returned.
 **/
HashMap* hashMapInit(key_hash_func_t key_hash_func,
		     key_cmp_func_t key_cmp_func,
	       	     HashMapEntryHandlers handlers);

/**
 * Removes all elements from the given map.
 **/
void hashMapClear(HashMap* map);

/**
 * Frees the given map object, and all elements contained in it.
 * Passing NULL has no effect.
 **/
void hashMapDestroy(HashMap* map);


/**
 * Inserts the key/value pair to the given map.
 * The values are copied by value using the functions that were passed to
 * hashMapInit.
 * 
 * If key already exists, its value is updated.
 * In case of a memroy allocation error, HASH_MAP_MEM_ERR is returned.
 * On success, HASH_MAP_SUCCESS is returned.
 **/
HashMapStatus hashMapInsert(HashMap* map, const void* key, const void* value);

/**
 * Checks whether the given map contains key.
 * Returns 1 if true, and 0 otherwise.
 **/
int hashMapContains(const HashMap* map, const void* key);

/**
 * Returns a reference to the value corresponding to the requested key.
 * If key doesn't exist, NULL is returned.
 **/
void* hashMapGet(HashMap* map, const void* key);

/**
 * Removes a key/value pair from the map.
 * This function has no effect if key doesn't exist
 **/
void hashMapRemove(HashMap* map, const void* key);


/**
 * Returns the size of the given map.
 **/
size_t hashMapSize(const HashMap* map);

typedef void (*for_each_func_t) (void* data, void* params);

/**
 * Applies func to every element in the map.
 **/
void hashMapForEach(HashMap* map, for_each_func_t func, void* params);


#endif // __HASH_MAP_H__
