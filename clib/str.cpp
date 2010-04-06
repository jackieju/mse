#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "str.h"
#include "assert.h"

char *trim_ends(char *str) {
	unsigned char *ustr, *end;
	
	ustr = (unsigned char *)str;
	while (isspace(*ustr))
		++ustr;
	end = (unsigned char *)strchr((char *)ustr, '\0');
	while (ustr < end && isspace(end[-1]))
		--end;
	*end = '\0';
	return (char *)ustr;
}


int count_occurences(char *str, char *pat) {
	int count;
	size_t len;
	
	count = 0;
	len = strlen(pat);
	while ((str = strstr(str, pat)) != NULL) {
		++count;
		str += len;
	}
	return count;
}


char *strndup(char *str, size_t n) {
	char *p;
	
	p = (char *)OS_malloc(n + 1);
	memcpy(p, str, n);
	p[n] = '\0';
	return p;
}


char *str_case_str(char *str, char *pat) {
	char *p, *s;
	
	while (*str != '\0') {
		for (p = pat, s = str; *p != '\0' && *s != '\0'; ++p, ++s)
			if (tolower(*p) != tolower(*s))
				break;
		if (*p == '\0')
			return str;
		++str;
	}
	return NULL;
}


/*
 * seek string 's' backward from offset 'start_offset'. Return offset of
 * the first occurance of any character in 'accept' string, or -1 if not
 * found  */
int str_reverse_seek(const char *s, int start_offset, const char *accept)
{
    char	*other;

    for(;start_offset >= 0; start_offset--) {
	for(other = (char *)accept; *other != '\0'; other++) {
	    if (*other == s[start_offset])
		return start_offset;
	}
    }
    return -1;		/* not found */
}


/* as above but ignoring case */
int str_reverse_case_seek(const char *s, int start_offset, const char *accept)
{
    char	*other;

    for(;start_offset >= 0; start_offset--) {
	for(other = (char *)accept; *other != '\0'; other++) {
	    if (toupper(*other) == toupper(s[start_offset]))
		return start_offset;
	}
    }
    return -1;		/* not found */
}

// strcp duplicate
char *os_strdup(const char *str)
{
    char *copy;

    if (!str)
		return  NULL;

    copy = new char[strlen(str) + 1];
    strcpy(copy, str);

    return copy;
}

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
char *strlwr(char *str)
{
	char *p = str;

	for (; p && *p ; p++)
	{
		if (*p >= 'A' && *p <= 'Z')
		{
			*p = 'a' + *p - 'A';
		}
	}

	return str;
}
#endif