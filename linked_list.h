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

typedef void* (*list_value_copy_func_t)(const void*);
typedef int (*list_value_cmp_func_t)(const void*, const void*);
typedef void (*list_value_free_func_t)(void*);


/**
 * Creates a new linked list object.
 * If a memory allocation error occurs, NULL is returned.
 **/
LinkedList* linkedListInit(list_value_copy_func_t value_copy_func,
			   list_value_cmp_func_t value_cmp_func,
			   list_value_free_func_t value_free_func);


/**
 * Copies the given list by reference.
 **/
LinkedList* linkedListCopy(const LinkedList* other);

/**
 * Returns the size of the given list.
 **/
size_t linkedListSize(const LinkedList* list);

/**
 * Checks if the given list contains an element equal to value.
 * If such an element is found, a reference to it is returned.
 * Otherwise, NULL is returns.
 **/
void* linkedListContains(const LinkedList* list, void* value);


/**
 * Returns a pointer to the element at the requested index.
 * If the index is greater that the size of the list, NULL is returned.
 * List indices start at 0.
 **/
void* linkedListGetAt(LinkedList* list, size_t index);


/**
 * Inserts the given value to the list.
 **/
LinkedListStatus linkedListInsert(LinkedList* list, void* value);

/**
 * If an element equal to value resides in the list, it it removed.
 * Otherwise, this call has no effect.
 **/
LinkedListStatus linkedListRemove(LinkedList* list, void* value);

/**
 * Frees a list object, and all elements in it.
 * Passing NULL has no effect.
 **/
void linkedListDestroy(LinkedList* list);

/**
 * Removes all elements from the given list.
 **/
void linkedListClear(LinkedList* list);

#endif // __LINKED_LIST__
