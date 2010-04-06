/*
 * ssassert.h - assertions macros that report via log files
 *
 * Define our own version of assert that calls panic(), because the
 * normal assert() prints to stdout which no-one will see.
 *
 * We also define a ss_assert_place macro so that we can easily use it
 * data structure consistency checking function and report the place where
 * the consistency checking function was called.
 *
 */

#include "log.h"  /* for panic() */

/* If NDEBUG is defined, assert does nothing. */
#ifdef WIN32
	#undef __FUNCTION__
	#define __FUNCTION__ "unknow function"
#endif

//temp use, TODO: delete these macros
#define OS_free(x)		free(x)
#define OS_malloc(x)	malloc(x)
//#define ss_realloc(ptr, size) realloc(ptr, size)


#define NDEBUG 1
#ifdef NDEBUG
#define OS_assert(expr) ((void) 0)
#define OS_assert_place(expr, file, lineno, func) ((void) 0)
#else
#define OS_assert(expr) \
	((void) ((expr) ? 0 : \
		  panic(0, "%s:%ld: %s: Assertion `%s' failed.", \
			__FILE__, (long) __LINE__, __FUNCTION__, #expr), 0))
#define OS_assert_place(expr, file, lineno, func) \
	((void) ((expr) ? 0 : \
		  panic(0, "%s:%ld: %s: Assertion `%s' failed. " \
		           "(Called from %s:%ld:%s.)", \
			      __FILE__, (long) __LINE__, __FUNCTION__, \
			      #expr, (file), (long) (lineno), (func)), 0))
#endif
