#include "parser.h"
#include "string.h"

#include "test_utils.h"



int test_parse_int_normal_case()
{
	char* error = NULL;
	char expr[20] = "20\n";
	assert_int_eq(20, parse_int(expr, &error));

	strcpy(expr, "-20\n");
	assert_int_eq(-20, parse_int(expr, &error));

	return 1;
}


int main()
{
	RUN_TEST(test_parse_int_normal_case);
	return 0;
}

