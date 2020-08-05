#ifndef __BUCKET_ENTRY_H__
#define __BUCKET_ENTRY_H__

typedef struct bucket Bucket;

Bucket* bucketCreate();
void bucketDestroy(Bucket* bucket);

int bucketContains(Bucket* bucket, void* key);
int bucketInsert(Bucket* bucket, void* key, void* value);

#endif // __BUCKET_ENTRY_H__

