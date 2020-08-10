#include <assert.h>
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
		entry->data = NULL;
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
}

struct linked_list
{
	ListEntry* dummy;
	size_t size;
	list_value_copy_func_t value_copy_func;
	list_value_cmp_func_t value_cmp_func;
	list_value_free_func_t value_free_func;
};


LinkedList* linkedListInit(list_value_copy_func_t value_copy_func,
			   list_value_cmp_func_t value_cmp_func,
			   list_value_free_func_t value_free_func)
{
	assert(value_copy_func);
	assert(value_cmp_func);
	assert(value_free_func);
	
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
		list->value_free_func = value_free_func;
	}
	return list;
}


LinkedList* linkedListCopy(LinkedList* other)
{
	return other;
}


size_t linkedListSize(const LinkedList* list)
{
	assert(list);
	return list->size;
}


static ListEntry* entryFind(LinkedList* list, void* data)
{
	assert(list);
	ListEntry* curr = list->dummy->next;
	while (curr)
	{
		if (0 == list->value_cmp_func(curr->data, data))
		{
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}


void* linkedListContains(const LinkedList* list, void* data)
{
	assert(list);
	// const cast is ok since the list already exists, and we're not modifying it.
	ListEntry* found = entryFind((LinkedList*)list, data);
	if (found)
	{
		return found->data;
	}
	return NULL;
}


void* linkedListGetAt(LinkedList* list, size_t index)
{
	assert(list);

	ListEntry* itr = list->dummy->next;
	while(itr && index)
	{
		itr = itr->next;
		--index;
	}
	
	if (itr) return itr->data;
	return NULL;
}


static void addLast(LinkedList* list, ListEntry* entry)
{
	assert(list);
	ListEntry* itr = list->dummy;
	while (itr->next) itr = itr->next;
	link(itr, entry);
}



LinkedListStatus linkedListInsert(LinkedList* list, void* data)
{
	assert(list);
	ListEntry* found = entryFind(list, data);
	void* new_value = list->value_copy_func(data);
	if (!new_value) return LINKED_LIST_MEM_ERROR;
	
	if (found)
	{
		list->value_free_func(found->data);
		found->data = new_value;
	}
	else
	{
		ListEntry* new_entry = entryCreate();
		if (!new_entry) return LINKED_LIST_MEM_ERROR;

		new_entry->data = new_value;
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


LinkedListStatus linkedListRemove(LinkedList* list, void* data)
{
	assert(list);
	ListEntry* found = entryFind(list, data);
	if (found)
	{
		unlink(found);
		list->value_free_func(found->data);
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
			list->value_free_func(curr->data);
			curr->data = NULL;
			ListEntry* temp = curr;
			curr = curr->next;
			entryDestroy(temp);
		}

		list->value_free_func(curr->data);
		entryDestroy(curr);
		free(list);
	}
}

