#include <malloc.h>

#include "string.h"
#include "linked_list.h"
#include "test_utils.h"

#include "string.h"

void* str_copy(void* s)
{
	return strdup(s);
}

void str_free(void* s)
{
	free(s);
}

int str_compare(void* s1, void* s2)
{
	return strcmp(s1, s2);
}


int test_sanity()
{
	LinkedList* list = linkedListInit(str_copy, str_compare, str_free);
	linkedListDestroy(list);
	return 1;
}

int test_size()
{
	LinkedList* list = linkedListInit(str_copy, str_compare, str_free);
	
	assert_int_eq(linkedListSize(list), 0);

	linkedListInsert(list, "abc");
	assert_int_eq(linkedListSize(list), 1);
	
	linkedListInsert(list, "abcd");
	assert_int_eq(linkedListSize(list), 2);
	
	linkedListRemove(list, "abcd");
	assert_int_eq(linkedListSize(list), 1);

	linkedListDestroy(list);
	return 1;
}


int test_get_at()
{
	LinkedList* list = linkedListInit(str_copy, str_compare, str_free);
	char* res = NULL;

	assert_null(linkedListGetAt(list, 100));
	assert_null(linkedListGetAt(list, 1));


	linkedListInsert(list, "abc");
	res = linkedListGetAt(list, 0);
	assert_str_eq(res, "abc");
	
	linkedListInsert(list, "abcd");
	res = linkedListGetAt(list, 0);
	assert_str_eq(res, "abc");
	res = linkedListGetAt(list, 1);
	assert_str_eq(res, "abcd");
	
	linkedListInsert(list, "abcde");
	res = linkedListGetAt(list, 0);
	assert_str_eq(res, "abc");
	
	res = linkedListGetAt(list, 1);
	assert_str_eq(res, "abcd");
	
	res = linkedListGetAt(list, 2);
	assert_str_eq(res, "abcde");

	linkedListDestroy(list);
	return 1;

}

int main()
{
	RUN_TEST(test_sanity);
	RUN_TEST(test_size);
	RUN_TEST(test_get_at);

	return 0;
}


