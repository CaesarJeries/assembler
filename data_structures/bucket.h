#ifndef __BUCKET_ENTRY_H__
#define __BUCKET_ENTRY_H__

typedef struct bucket Bucket;

Bucket* bucketCreate();
void bucketDestroy(Bucket* entry);

int bucketContains(void* key);
int bucketInsert(void* key, void* value);

#endif // __BUCKET_ENTRY_H__

