#ifndef __TEST_UTILS_H__
#define __TEST_UTILS_H__

#include <stdio.h>


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


#endif // __TEST_UTILS_H__

