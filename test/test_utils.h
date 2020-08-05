#ifndef __TEST_UTILS_H__
#define __TEST_UTILS_H__

#include <stdio.h>
#include <string.h>


#define assert_int_eq(a, b) do{\
	int v1 = a; int v2 = b;\
	if (v1 != v2) return 0;\
}while(0)


#define RUN_TEST(t) do{\
	int result = t();\
	if (result != 1){\
		fprintf(stderr, "Test failed: " #t "\n");\
	}else{\
		fprintf(stdout, "Test passed: " #t "\n");\
	}\
}while(0)


#define assert_null(val) do{\
	if ((val) != NULL) return 0; \
}while(0)

#define assert_not_null(val) do{\
	if ((val) == NULL) return 0; \
}while(0)


#define assert_str_eq(actual, expected) do{\
	if (0 != strcmp(actual, expected)){\
		fprintf(stderr, "Strings not equal. Expexted: %s, actual: %s\n", expected, actual);\
		return 0;\
	}\
}while(0)


#endif // __TEST_UTILS_H__

