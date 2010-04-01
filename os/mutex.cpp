
/**
 * mutex.cpp
 * for internal lock and unlock usage
 * Zhang Yuan
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifndef WIN32
 #include <unistd.h>
 //#if !HAVE_PTHREAD_H
 //#error "You need Posix threads and <pthread.h>"
 //#endif
 #include <pthread.h>
#else
 #include <windows.h>
 #include <process.h>
#endif

#include "thread.h"
#include "assert.h"
#include "mutex.h"
#include "log.h"

#ifdef MUTEX_STATS
Mutex *mutex_make_measured(Mutex *mutex, unsigned char *filename, int lineno)
{
    mutex->filename = filename;
    mutex->lineno = lineno;
    mutex->locks = 0;
    mutex->collisions = 0;
    return mutex;
}
#endif

Mutex *mutex_create_real(void)
{
    Mutex *mutex;

    mutex = (Mutex *)OS_malloc(sizeof(Mutex));
#ifndef WIN32
    pthread_mutex_init(&mutex->mutex, NULL);
#else
	InitializeCriticalSection(&mutex->mutex);
#endif
    mutex->owner = -1;
    mutex->dynamic = 1;
    return mutex;
}

Mutex *mutex_init_static_real(Mutex *mutex)
{
#ifndef WIN32
    pthread_mutex_init(&mutex->mutex, NULL);
#else
	InitializeCriticalSection(&mutex->mutex);
#endif
    mutex->owner = -1;
    mutex->dynamic = 0;
    return mutex;
}

void mutex_destroy(Mutex *mutex)
{
    if (mutex == NULL)
        return;

#ifdef MUTEX_STATS
    if (mutex->locks > 0 || mutex->collisions > 0) {
        info(0, "Mutex %s:%d: %ld locks, %ld collisions.",
             mutex->filename, mutex->lineno,
             mutex->locks, mutex->collisions);
    }
#endif

#ifndef WIN32
    pthread_mutex_destroy(&mutex->mutex);
#else
	DeleteCriticalSection(&mutex->mutex);
#endif
    if (mutex->dynamic == 0)
        return;
    OS_free(mutex);
}


void mutex_lock(Mutex *mutex)
{
    int ret = 0;

    OS_assert(mutex != NULL);
#ifndef WIN32
#ifdef MUTEX_STATS
    ret = pthread_mutex_trylock(&mutex->mutex);
    if (ret != 0) {
        ret = pthread_mutex_lock(&mutex->mutex);
        mutex->collisions++;
    }
    mutex->locks++;
#else
    ret = pthread_mutex_lock(&mutex->mutex);
#endif
#else
	EnterCriticalSection(&mutex->mutex);
#endif
    if (ret != 0)
        panic(ret, "mutex_lock: Mutex failure!");
	/*
	 * Added for compatibility with WIN32, Zhang Yuan, 2000-12-27.
	 */
	if (OSThreadSelf() == -1)
		return;
	/* End of the modification */

    if (mutex->owner == OSThreadSelf())
        panic(0, "mutex_lock: Managed to lock the mutex twice!");
    mutex->owner = OSThreadSelf();
}


int mutex_try_lock(Mutex *mutex)
{
    int ret = 0;
#ifndef WIN32

    OS_assert(mutex != NULL);

    /* Let's try to lock it. */
    ret = pthread_mutex_trylock(&mutex->mutex);

    if (ret == EBUSY)
        return -1;  /* Oops, didn't succeed, someone else locked it. */

    if (ret != 0) {
        /* Oops, some real error occured. The only known case
         * where this happens is when a mutex object is
         * initialized badly or not at all. In that case,
         * we're stupid and don't deserve to live. */
        panic(ret, "mutex_try_lock: Mutex failure!");
    }

    if (mutex->owner == OSThreadSelf()) {
        /* The lock succeeded, but some thread systems allow
         * the locking thread to lock it a second time.  We
         * don't want that because it's not portable, so we
         * pretend it didn't happen. */
        pthread_mutex_unlock(&mutex->mutex);
        return -1;
    }

    /* Hey, it's ours! Let's remember that... */
    mutex->owner = OSThreadSelf();
    return 0;
#else
	return -1;
#endif
}


void mutex_unlock(Mutex *mutex)
{
    int ret = 0;
    OS_assert(mutex != NULL);
    mutex->owner = -1;
#ifndef WIN32
    ret = pthread_mutex_unlock(&mutex->mutex);
    if (ret != 0)
        panic(ret, "mutex_unlock: Mutex failure!");
#else
	LeaveCriticalSection(&mutex->mutex);
#endif
}
