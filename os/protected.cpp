/*
 * protected.cpp - thread-safe versions of standard library functions
 */
//#include "../config.h"

#ifndef WIN32
	#include <locale.h>
	//socket
	#include <netdb.h>
#else
	#include <winsock2.h>
	#include <ws2tcpip.h>
#endif

#include <time.h>
#include <stdlib.h>

#include "mutex.h"

// reference counter
static int iInitilizated = 0;

enum {
	LOCALTIME,
	GMTIME,
	RAND,
	GETHOSTBYNAME,
	NUM_LOCKS
};

static Mutex locks[NUM_LOCKS];

static void lock(int which) {
	mutex_lock(&locks[which]);
}


static void unlock(int which) {
	mutex_unlock(&locks[which]);
}


// critical section initilization
void OSlib_protected_init(void) 
{
	int i;

	if (iInitilizated++)
		return;

	for (i = 0; i < NUM_LOCKS; ++i)
		mutex_init_static(&locks[i]);
}

// critical section destruction
void OSlib_protected_shutdown(void) 
{
	int i;

	if (--iInitilizated)
		return;

	for (i = 0; i < NUM_LOCKS; ++i)
		mutex_destroy(&locks[i]);
}

struct tm OS_localtime(time_t t) {
	struct tm tm;

	lock(LOCALTIME);
	tm = *localtime(&t);
	unlock(LOCALTIME);
	return tm;
}

// TODO: check the necessarity of lock
struct tm OS_gmtime(time_t t) 
{
	struct tm tm;

	lock(GMTIME);
	tm = *gmtime(&t);
	unlock(GMTIME);
	return tm;
}


int OS_rand(void) {
	int ret;
	
	lock(RAND);
	ret = rand();
	unlock(RAND);
	return ret;
}


int OS_gethostbyname(struct hostent *ent, const char *name) {
	int ret = -1;
	struct hostent *p;
	
//	lock(GETHOSTBYNAME);
	p = gethostbyname(name);
	if (p == NULL)
		ret = -1;
	else {
		ret = 0;
		*ent = *p;
	}
//	unlock(GETHOSTBYNAME);
	return ret;
}
