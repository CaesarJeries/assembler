#include <assert.h>
#include <malloc.h>
#include "hash_map.h"

static const float DEFAULT_LOAD_FACTOR = 0.75;

typedef struct bucket_entry
{
	void* key;
	void* value;
	struct bucket_entry* next;
} Entry;


typedef struct bucket
{
	Entry* dummy;
} Bucket;


Entry* bucketEntryCreate()
{
	Entry* entry = malloc(sizeof(*entry));
	if (entry)
	{
		entry->key = NULL;
		entry->value = NULL;
		entry->next = NULL;
	}
	return entry;
}

Bucket* bucketCreate()
{
	Bucket* bucket = malloc(sizeof(*bucket));
	if (bucket)
	{
		bucket->dummy = bucketEntryCreate();
		if (!bucket->dummy)
		{
			free(bucket);
			return NULL;
		}
	}
	return bucket;
}


void bucketDestroy(Bucket* bucket, HashMapEntryHandlers handlers)
{
	if (!bucket) return;

	Entry* itr = bucket->dummy->next;
	while (itr)
	{
		handlers.key_free(itr->key);
		handlers.value_free(itr->value);
		Entry* temp = itr;
		itr = itr->next;
		free(temp);
	}
}


struct hash_map
{
	Bucket** buckets;
	size_t num_buckets;
       	size_t num_elements;	
	float load_factor;
	HashMapEntryHandlers handlers;
	key_hash_func_t key_hash_func;
	key_cmp_func_t key_cmp_func;
};


Bucket** createBuckets(size_t size)
{
	size_t total_size = size * sizeof(Bucket*);
	Bucket** buckets = calloc(1, total_size);
	if (buckets)
	{

		for (size_t i = 0; i < size; ++i)
		{
			buckets[i] = bucketCreate();
			if (!buckets[i])
			{
				return NULL;
			}
		}
	}
	return buckets;
}


HashMap* hashMapInit(key_hash_func_t key_hash_func,
		     key_cmp_func_t key_cmp_func,
	       	     HashMapEntryHandlers handlers)
{
	assert (key_hash_func);
	assert (key_cmp_func);

	HashMap* map = malloc(sizeof(*map));
	if (map)
	{
		const size_t default_num_buckets = 32; 
		map->num_buckets = default_num_buckets;
		map->num_elements = 0;
		map->load_factor = 0;
		map->key_hash_func = key_hash_func;
		map->key_cmp_func = key_cmp_func;
		map->handlers = handlers;
		
		Bucket** buckets = createBuckets(map->num_buckets);
		if (buckets)
		{
			map->buckets = buckets;
		}
		else
		{
			hashMapDestroy(map);
			map = NULL;
		}
	}

	return map;
}


void hashMapDestroy(HashMap* map)
{
	if (!map) return;
	
	for (size_t i = 0; i < map->num_buckets; ++i)
	{
		bucketDestroy(map->buckets[i], map->handlers);
	}

	free(map->buckets);
	free(map);
}


static Entry* findBucketEntry(Bucket* bucket, void* key, key_cmp_func_t key_cmp_func)
{
	Entry* itr = bucket->dummy->next;
	while (itr)
	{
		if (0 == key_cmp_func(key, itr->key))
		{
			return itr;
		}
	}
	return NULL;
}


static void addLast(Bucket* bucket, Entry* new_entry)
{
	Entry* itr = bucket->dummy;
	while (itr->next) itr = itr->next;
	itr->next = new_entry;
}


static void updateLoadFactor(HashMap* map)
{
	map->load_factor = map->num_elements / (float)map->num_buckets;
}


static HashMapErrorCode resizeHashMap(HashMap* map)
{
	size_t new_size = 2 * map->num_buckets;
	Bucket** new_buckets = createBuckets(new_size);
	if (!new_buckets)
	{
		return HASH_MAP_MEM_ERROR;
	}

	Bucket** old_buckets = map->buckets;
	size_t old_num_buckets = map->num_buckets;
	
	// reset statistics in the original hash table
	map->num_buckets = new_size;
	map->buckets = new_buckets;
	map->load_factor = 0;
	map->num_elements = 0;

	// rearrange old entries into the new table.
	// this avoids unnecessary copy operations.
	for (size_t i = 0; i < old_num_buckets; ++i)
	{
		Entry* itr = old_buckets[i]->dummy->next;
		while (itr)
		{
			Entry* next = itr->next;
			itr->next = NULL;
			size_t hash = map->key_hash_func(itr->key, map->num_buckets);
			addLast(map->buckets[hash], itr);
			itr = next;
			++map->num_elements;
			updateLoadFactor(map);
		}
		old_buckets[i]->dummy->next = NULL;
	}

	return HASH_MAP_SUCCESS;
}


int hashMapInsert(HashMap* map, void* key, void* value)
{
	size_t hash = map->key_hash_func(key, map->num_buckets);
	
	void* new_value = map->handlers.value_copy(value);
	if (!new_value) return HASH_MAP_MEM_ERROR;
	
	Entry* bucket_entry = findBucketEntry(map->buckets[hash], key, map->key_cmp_func);
	if (bucket_entry)
	{
		
		map->handlers.value_free(bucket_entry->value);
		bucket_entry->value = new_value;
	}
	else
	{
		Entry* new_entry = malloc(sizeof(*new_entry));
		if (!new_entry)
		{
			map->handlers.value_free(new_value);
			return HASH_MAP_MEM_ERROR;
		}
		
		new_entry->key = map->handlers.key_copy(key);
		if (!new_entry->key)
		{
			free(new_entry);
			map->handlers.value_free(new_value);
			return HASH_MAP_MEM_ERROR;
		}

		new_entry->value = new_value;
		addLast(map->buckets[hash], new_entry);
		++map->num_elements;
		updateLoadFactor(map);

		if (map->load_factor > DEFAULT_LOAD_FACTOR)
		{
			return resizeHashMap(map);
		}
	}
	return HASH_MAP_SUCCESS;
}

