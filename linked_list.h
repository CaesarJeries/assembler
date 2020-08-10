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

typedef void* (*list_value_copy_func_t)(void*);
typedef int (*list_value_cmp_func_t)(void*, void*);
typedef void (*list_value_free_func_t)(void*);

LinkedList* linkedListInit(list_value_copy_func_t value_copy_func,
			   list_value_cmp_func_t value_cmp_func,
			   list_value_free_func_t value_free_func);


LinkedList* linkedListCopy(LinkedList* other);


size_t linkedListSize(const LinkedList* list);
void* linkedListContains(const LinkedList* list, void* value);

void* linkedListGetAt(LinkedList* list, size_t index);

LinkedListStatus linkedListInsert(LinkedList* list, void* value);
LinkedListStatus linkedListRemove(LinkedList* list, void* value);

void linkedListDestroy(LinkedList* list);


#endif // __LINKED_LIST__
