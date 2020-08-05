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
	char* str = itoa(3);
	assert_str_eq(str, "3");
	free(str);

	str = itoa(-7);
	assert_str_eq(str, "-7");
	free(str);

	return 1;
}


int main()
{
	RUN_TEST(test_strdup);
	RUN_TEST(test_itoa);
	return 0;
}

