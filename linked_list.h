#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

#include <stddef.h>	// size_t

typedef enum
{
	LINKED_LIST_SUCCESS,
	LINKED_LIST_MEM_ERROR,
	LINKED_LIST_ELEM_NOT_FOUND
} LinkedListStatus;




typedef struct linked_list LinkedList;

typedef void* (*value_copy_func_t)(void*);
typedef int (*value_cmp_func_t)(void*, void*);
typedef void (*value_free_func_t)(void*);

LinkedList* linkedListInit(value_copy_func_t value_copy_func,
			   value_cmp_func_t value_cmp_func,
			   value_free_func_t value_free_func);



size_t linkedListSize(const LinkedList* list);
void* linkedListContains(const LinkedList* list, void* value);

void* linkedListGetAt(LinkedList* list, size_t index);

LinkedListStatus linkedListInsert(LinkedList* list, void* value);
LinkedListStatus linkedListRemove(LinkedList* list, void* value);

void linkedListDestroy(LinkedList* list);


#endif // __LINKED_LIST__
