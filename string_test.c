#include <malloc.h>

#include "string.h"
#include "test_utils.h"


int test_strdup()
{
	const char* s = "abcd";
	char* str = strdup(s);
	assert_str_eq(str, "abcd");
	assert (str != s); // check addresses
	free(str);
	return 1;
}


int test_itoa()
{
	static char str[50] = {0};
	itoa(str, 3);
	assert_str_eq(str, "3");

	itoa(str, -7);
	assert_str_eq(str, "-7");

	return 1;
}


int main()
{
	RUN_TEST(test_strdup);
	RUN_TEST(test_itoa);
	return 0;
}

