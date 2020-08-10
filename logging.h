#ifndef __LOGGING_H__
#define __LOGGING_H__

#define HELPER(fmt, ...) printf("%s" fmt "\n", __VA_ARGS__)
#define VA_ARGS(fmt, ...) HELPER(fmt, "", __VA_ARGS__)

#define error(fmt, ...) VA_ARGS(fmt, ##__VA_ARGS__)
#define report_error(s) printf(s); printf("\n")
#define _debug(fmt, ...) VA_ARGS(fmt, ##__VA_ARGS__)


#ifndef NDEBUG
#define debug(fmt, ...) _debug("file: %s, line: %d: " fmt, \
		__FILE__, __LINE__, ##__VA_ARGS__)

#else
#define debug(fmt, ...) 
#endif	// NDEBUG

#endif	// __LOGGING_H__
