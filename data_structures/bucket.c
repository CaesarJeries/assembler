#include "bucket.h"



void bucketDestroy(Bucket* bucket)
{
	free(bucket);
}

int bucketContains(Bucket* bucket, void* key)
{
	Entry* itr = bucket->dummy;
	while (itr->next)
	{

int bucketInsert(void* key, void* value);
