

#ifndef _OSSTR_H__
#define _OSSTR_H__

#include "osmacros.h"

#define OSstrcmp(x, y)		strcmp((x), (y))
#define OSstrncmp(x, y, z)	strncmp((x), (y), (z))
#define OSstrstr(x, y)		strstr((x), (y))
#define OSstrchr(x, y)		strchr((x), (y))
#define OSstrrchr(x, y)		strrchr((x), (y))
#define OSstrcpy(x, y)		strcpy((x), (y))

#ifndef WIN32
#define OSstricmp(x, y)		strcasecmp(x, y)
#define OSstrnicmp(x, y, z)	strncase(x, y, z)
#else
#define OSstricmp(x, y)		stricmp(x, y)
#define OSstrnicmp(x, y, z)	strnicmp(x, y, z)
#endif

/**
 * <pre>
 * string duplicate operation.
 * Parameters: 
 *				str, source string
 * Return value:
 *				result or destination string
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD char *os_strdup(const char *str);

/**
 * <pre>
 * Remove leading and trailing whitespace.
 * Parameters: 
 *				str, source string
 * Return value:
 *				result or destination string
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD char *trim_ends(char *str);

/**
 * <pre>
 * Count the number of times `pat' occurs on `str'.
 * Parameters: 
 *				str, source string
 *				pat, search pattern
 * Return value:
 *				counter of occurenced of searching pattern, pat
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int count_occurences(char *str, char *pat);

/**
 * <pre>
 * Make a dynamically allocated copy of first `n' characters of `str'.
 * Parameters: 
 *				str, source string
 *				n, the number of character will be copied.
 * Return value:
 *				result or destination string
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD char *strndup(char *str, size_t n);

/**
 * <pre>
 * like strstr, but ignore case.
 * Parameters: 
 *				str, source string
 *				pat, search pattern
 * Return value:
 *				result or destination string
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD char *str_case_str(char *str, char *pat);

#ifndef WIN32
/**
 * <pre>
 * Convert a string to lowercase.
 * Parameters: 
 *				str, source string
 * Return value:
 *				result string, i.e. the old string
 * </pre>
 */
char *strlwr(char *str);
#endif

/**
 * <pre>
 * Seek string 's' backward from offset 'start_offset'.
 * Parameters: 
 *				s, source string
 *				start_offset, search starting point
 *				accept, string pattern to be searched
 * Return value:
 *				Return offset of the first occurance of any character in 'accept' string, or -1 if not
 * found
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int str_reverse_seek(const char *s, int start_offset, const char *accept);

/**
 * <pre>
 * As above but ignoring case.
 * Parameters: 
 *				s, source string
 *				start_offset, search starting point
 *				accept, string pattern to be searched
 * Return value:
 *				Return offset of the first occurance of any character in 'accept' string, or -1 if not
 * found
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int str_reverse_case_seek(const char *s, int start_offset, 
    	    	    	  const char *accept);
#endif
