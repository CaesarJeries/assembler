#include <malloc.h>

#include "linked_list.h"
#include "parser.h"
#include "string.h"
#include "test_utils.h"


int test_parse_int_normal_case()
{
	char* error = NULL;
	char expr[20] = "20\n";
	assert_int_eq(20, parse_int(expr, &error));
	assert_null(error);

	strcpy(expr, "-20\n");
	assert_int_eq(-20, parse_int(expr, &error));
	assert_null(error);
	
	strcpy(expr, "-000020\n");
	assert_int_eq(-20, parse_int(expr, &error));
	assert_null(error);

	return 1;
}


int test_parse_int_error_case()
{
	char* error = NULL;
	char expr[20] = "+\n";
	parse_int(expr, &error);
	assert_not_null(error);

	strcpy(expr, "\n");
	parse_int(expr, &error);
	assert_not_null(error);
	
	strcpy(expr, "-\n");
	parse_int(expr, &error);
	assert_not_null(error);
	
	strcpy(expr, "-123ab\n");
	parse_int(expr, &error);
	assert_not_null(error);
	
	strcpy(expr, "illegal characters");
	parse_int(expr, &error);
	assert_not_null(error);

	return 1;
}	


int test_parse_string_normal_case()
{
	char* error = NULL;
	char* str = parse_string("\"abc\"", &error);
	assert_str_eq(str, "abc");
	assert_null(error);
	free(str);
	
	str = parse_string("\"\"", &error);
	assert_str_eq(str, "");
	assert_null(error);
	free(str);

	return 1;
}

int test_parse_string_error_case()
{
	char* error = NULL;
	char* str = parse_string("aaaa", &error);
	assert_null(str);
	assert_not_null(error);
	
	str = parse_string("\"abc\n\"", &error);
	assert_null(str);
	assert_not_null(error);

	return 1;
}


int test_parse_data()
{
	char* error = NULL;
	LinkedList* list = parse_data("45, -13, 42", &error);
	assert_int_eq(linkedListSize(list), 3);

	linkedListDestroy(list);
	return 1;
}


int main()
{
	RUN_TEST(test_parse_int_normal_case);
	RUN_TEST(test_parse_int_error_case);
	
	RUN_TEST(test_parse_string_normal_case);
	RUN_TEST(test_parse_string_error_case);
	return 0;
}

