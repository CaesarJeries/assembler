#include <assert.h>
#include <malloc.h>

#include "hash_map.h"
#include "logging.h"

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


static void bucketClear(Bucket* bucket, HashMapEntryHandlers handlers)
{
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

void bucketDestroy(Bucket* bucket, HashMapEntryHandlers handlers)
{
	if (!bucket) return;
	
	bucketClear(bucket, handlers);
	free(bucket->dummy);
	free(bucket);
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


static Bucket** createBucketArray(size_t size, HashMapStatus* status)
{
	assert (status);
	size_t total_size = size * sizeof(Bucket*);
	Bucket** buckets = calloc(1, total_size);
	if (buckets)
	{

		for (size_t i = 0; i < size; ++i)
		{
			buckets[i] = bucketCreate();
			if (!buckets[i])
			{
				*status = HASH_MAP_MEM_ERROR;	
			}
		}
	}
	return buckets;
}


static void destroyBucketArray(Bucket** buckets,
			       size_t size,
			       HashMapEntryHandlers handlers)
{
	if (buckets)
	{	
		for (size_t i = 0; i < size; ++i)
		{
			bucketDestroy(buckets[i], handlers);
		}
		free(buckets);
	}
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
		
		HashMapStatus status = HASH_MAP_SUCCESS;	
		Bucket** buckets = createBucketArray(map->num_buckets, &status);
		map->buckets = buckets;
		if (HASH_MAP_SUCCESS != status)
		{
			hashMapDestroy(map);
			map = NULL;
		}
	}

	return map;
}

void hashMapClear(HashMap* map)
{
	size_t size = map->num_buckets;
	
	for (size_t i = 0; i < size; ++i)
	{
		bucketClear(map->buckets[i], map->handlers);
	}
}


void hashMapDestroy(HashMap* map)
{
	if (!map) return;
	destroyBucketArray(map->buckets, map->num_buckets, map->handlers);
	free(map);
}


static Entry* findBucketEntry(Bucket* bucket, const void* key, key_cmp_func_t key_cmp_func)
{
	Entry* itr = bucket->dummy->next;
	while (itr)
	{
		if (0 == key_cmp_func(key, itr->key))
		{
			return itr;
		}
		itr = itr->next;
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


static HashMapStatus resizeHashMap(HashMap* map)
{
	debug("resizing map");
	size_t new_size = 2 * map->num_buckets;
	HashMapStatus status = HASH_MAP_SUCCESS;
	Bucket** new_buckets = createBucketArray(new_size, &status);
	if (HASH_MAP_SUCCESS != status)
	{
		destroyBucketArray(new_buckets, new_size, map->handlers);
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

	destroyBucketArray(old_buckets, old_num_buckets, map->handlers);
	return HASH_MAP_SUCCESS;
}


HashMapStatus hashMapInsert(HashMap* map, const void* key, const void* value)
{
	size_t hash = map->key_hash_func(key, map->num_buckets);
	
	void* new_value = map->handlers.value_copy(value);
	if (!new_value) return HASH_MAP_MEM_ERROR;
	
	Entry* bucket_entry = findBucketEntry(map->buckets[hash], key, map->key_cmp_func);
	if (bucket_entry)
	{
		debug("Updating existing entry");	
		map->handlers.value_free(bucket_entry->value);
		bucket_entry->value = new_value;
	}
	else
	{
		debug("Adding new entry");
		Entry* new_entry = bucketEntryCreate(); 
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
	
	debug("%s", "Entry added");
	return HASH_MAP_SUCCESS;
}

void* hashMapGet(HashMap* map, const void* key)
{
	size_t hash = map->key_hash_func(key, map->num_buckets);
	Bucket* bucket = map->buckets[hash];
	if (!bucket) return NULL;

	Entry* itr = bucket->dummy->next;
	while (itr)
	{
		if (0 == map->key_cmp_func(key, itr->key))
		{
			return itr->value;
		}
		itr = itr->next;
	}

	return NULL;
}

size_t hashMapSize(const HashMap* map)
{
	return map->num_elements;
}

void hashMapRemove(HashMap* map, const void* key)
{
	size_t hash = map->key_hash_func(key, map->num_buckets);
	Bucket* bucket = map->buckets[hash];
	Entry* target = findBucketEntry(bucket, key, map->key_cmp_func);
	if (target)
	{
		Entry* itr = bucket->dummy;
		while (itr->next != target) itr = itr->next;
		itr->next = target->next;
		target->next = NULL;
		map->handlers.key_free(target->key);
		map->handlers.value_free(target->value);
		free(target);
		--map->num_elements;
		updateLoadFactor(map);
	}
}

void hashMapForEach(HashMap* map, for_each_func_t func, void* params)
{
	for (size_t bucket_itr = 0; bucket_itr < map->num_buckets; ++bucket_itr)
	{
		Bucket* bucket = map->buckets[bucket_itr];
		if (!bucket) continue;
		
		Entry* entry_itr = bucket->dummy->next;

		while (entry_itr)
		{
			func(entry_itr->value, params);
			entry_itr = entry_itr->next;
		}
	}
}


