#include <malloc.h>
#include <stdio.h>
#include "hash_map.h"


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


#define RUN_TEST(t) do{\
	int result = t();\
	if (result != 1){\
		fprintf(stderr, "Test failed: " #t "\n");\
	}else{\
		fprintf(stdout, "Test passed: " #t "\n");\
	}\
}while(0)


int main()
{
	RUN_TEST(test_sanity);
	return 0;
}
