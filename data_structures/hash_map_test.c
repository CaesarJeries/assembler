#include <malloc.h>
#include <stdio.h>

#include "hash_map.h"
#include "test_utils.h"


void* copy_int(void* n)
{
	int* value = malloc(sizeof(int));
	*value = *((int*)n);
	return value;
}

int compare_int(void* a, void* b)
{
	int val_a = *(int*)a;
	int val_b = *(int*)b;
	return val_a - val_b;
}

void free_int(void* value)
{
	free(value);
}

size_t hash_int(void* value, size_t size)
{
	int val = *(int*)value;
	return val % size;
}

HashMapEntryHandlers handlers = {copy_int, free_int, copy_int, free_int};


int test_sanity()
{
	HashMap* map = hashMapInit(hash_int,
				   compare_int,
				   handlers);
	hashMapDestroy(map);
	return 1;
}

int test_insert()
{
	HashMap* map = hashMapInit(hash_int,
				   compare_int,
				   handlers);
	int k = 0;
	int v = 100;	
	int retval = hashMapInsert(map, &k, &v);
	assert_int_eq(HASH_MAP_SUCCESS, retval);
	assert_int_eq(hashMapSize(map), 1);
	int value = *(int*)hashMapGet(map, &k);
	assert_int_eq(value, v);
	
	v = 200;
	retval = hashMapInsert(map, &k, &v);
	assert_int_eq(HASH_MAP_SUCCESS, retval);
	assert_int_eq(hashMapSize(map), 1);
	value = *(int*)hashMapGet(map, &k);
	assert_int_eq(value, v);
	
	hashMapDestroy(map);
	return 1;
}


int test_insert_resize()
{
	HashMap* map = hashMapInit(hash_int,
				   compare_int,
				   handlers);
	for (int i = 0; i < 34; ++i)
	{
		int retval = hashMapInsert(map, &i, &i);
		assert_int_eq(HASH_MAP_SUCCESS, retval);
		assert_int_eq(hashMapSize(map), i + 1);
	}
	
	hashMapDestroy(map);
	return 1;
}



int test_get()
{
	HashMap* map = hashMapInit(hash_int,
				   compare_int,
				   handlers);
	for (int i = 0; i < 34; ++i)
	{
		hashMapInsert(map, &i, &i);
		int value = *(int*)hashMapGet(map, &i);
		assert_int_eq(value, i);
	}

	for (int i = 0; i < 34; ++i)
	{
		int new_value = 2*i;
		hashMapInsert(map, &i, &new_value);
		int value = *(int*)hashMapGet(map, &i);
		assert_int_eq(value, new_value);
	}

	
	hashMapDestroy(map);
	return 1;
}


int test_size()
{
	HashMap* map = hashMapInit(hash_int,
				   compare_int,
				   handlers);
	int size = hashMapSize(map);
	assert_int_eq(size, 0);

	for (int i = 0; i < 34; ++i)
	{
		hashMapInsert(map, &i, &i);
		size = hashMapSize(map);
		assert_int_eq(size, i + 1);
	}

	for (int i = 33; i >= 0; --i)
	{
		hashMapRemove(map, &i);
		size = hashMapSize(map);
		assert_int_eq(size, i);
	}

	hashMapDestroy(map);
	return 1;

}


int main()
{
	RUN_TEST(test_sanity);
	RUN_TEST(test_insert);
	RUN_TEST(test_insert_resize);
	RUN_TEST(test_get);
	RUN_TEST(test_size);
	return 0;
}
