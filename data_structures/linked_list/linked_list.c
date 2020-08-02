#include <malloc.h>
#include "linked_list.h"

typedef struct list_entry
{
	void* data;
	struct list_entry* next;
	struct list_entry* prev;
} ListEntry;


static ListEntry* entryCreate()
{
	ListEntry* entry = malloc(sizeof(*entry));
	if (entry)
	{
		entry-data = NULL;
		entry->next = entry->prev = NULL;
	}
	return entry;
}

static void entryDestroy(ListEntry* entry)
{
	free (entry);
}

static void link(ListEntry* first, ListEntry* second)
{
	first->next = second;
	second->prev = first;


struct linked_list
{
	ListEntry* dummy;
	size_t size;
	value_copy_func_t value_copy_func;
	value_cmp_func_t value_cmp_func;
	value_free_func_t value_free_func;
};


LinkedList* linkedListInit(value_copy_func_t value_copy_func,
			   value_cmp_func_t value_cmp_func,
			   value_free_func_t value_free_func)
{
	LinkedList* list = malloc(sizeof(*list));
	if (list)
	{
		list->dummy = entryCreate();
		if (!list->dummy)
		{
			free(list);
			return NULL;
		}
		list->size = 0;
		list->value_copy_func = value_copy_func;
		list->value_cmp_func = value_cmp_func;
	}
	return list;
}


size_t linkedListSize(const LinkedList* list)
{
	return list->size;
}


static ListEntry* entryFind(LinkedList* list, void* value)
{
	ListEntry* curr = list->dummy->next;
	while (curr)
	{
		if (0 == list->value_cmp_func(curr->value, value))
		{
			return curr;
		}
	}
	return NULL;
}


void* linkedListContains(LinkedList* list, void* value)
{
	void* found = NULL;

	return found;
}


static void addLast(LinkedList* list, ListEntry* entry)
{
	ListEntry* itr = list->dummy;
	while (itr->next) itr = itr->next;
	link(itr, entry);
}



int linkedListInsert(LinkedList* list, void* value)
{
	ListEntry* found = entryFind(list, value);
	void* new_value = list->value_copy_func(value);
	if (!new_value) return LINKED_LIST_MEM_ERROR;
	
	if (found)
	{
		list->value_free_func(found->value);
		found->value = new_value;
	}
	else
	{
		ListEntry* new_entry = entryCreate();
		if (!new_entry) return LINKED_LIST_MEM_ERROR;

		new_entry->value = new_value;
		addLast(list, new_entry);
	}
	++list->size;
	return LINKED_LIST_SUCCESS;
}


static void unlink(ListEntry* entry)
{
	if (entry->next) entry->next->prev = entry->prev;
	if (entry->prev) entry->prev->next = entry->next;
}


int linkedListRemove(LinkedList* list, void* value)
{
	ListEntry* found = entryFind(list, value);
	if (found)
	{
		unlink(found);
		list->value_free_func(found->value);
		entryDestroy(found);
		--list->size;
		return LINKED_LIST_SUCCESS;
	}
	return LINKED_LIST_ELEM_NOT_FOUND;
}


void linkedListDestroy(LinkedList* list)
{
	if (list)
	{
		ListEntry* curr = list->dummy;
		while (curr->next)
		{
			list->value_free_func(curr->value);
			curr->value = NULL;
			curr = curr->next;
		}

		list->value_free_func(curr->value);
		free(list);
	}
}

