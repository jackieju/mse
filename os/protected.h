/*
 * protected.h - thread-safe versions of standard library functions
 *
 * The standard (or commonly available) C library functions are not always
 * thread-safe, or re-entrant. This module provides wrappers. The wrappers
 * are not always the most efficient, but the interface is meant to 
 * allow a more efficient version be implemented later.
 *
 */

#ifndef PROTECTED_H
#define PROTECTED_H

#ifndef WIN32
#include <netdb.h>
#endif
#include <time.h>

void OSlib_protected_init(void);
void OSlib_protected_shutdown(void);
struct tm OS_localtime(time_t t);
struct tm OS_gmtime(time_t t);
int OS_rand(void);
int OS_gethostbyname(struct hostent *ret, const char *name);

#endif
