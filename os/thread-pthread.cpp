/*
 * thread-pthread.c - implementation of thread.h using POSIX threads.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#ifndef WIN32
	#include <unistd.h>
	#include <errno.h>
	#include <pthread.h>
	#include <sys/time.h>
        #include <unistd.h>
	#include <sys/poll.h>
#else
	#include <windows.h>
	#include <process.h>
	#include <io.h>
#endif

#include "osmacros.h"
#include "assert.h"
#include "thread.h"


// external functions
// I placed it here, just because I don't want to include those many header files. : (
void OSlib_protected_init(void);

/* Maximum number of live threads we can support at once.  Increasing
 * this will increase the size of the threadtable.  Use powers of two
 * for efficiency. */
#define THREADTABLE_SIZE 1024
#define MAX_WAIT_TIME_LENGTH	5*1000

struct threadinfo {
#ifndef WIN32
	pthread_t self;
#else
	long self;
#endif
	const char *name;
	OSthread_func_t *func;
	long number;

#ifndef WIN32
	int wakefd_recv;
	int wakefd_send;
	pthread_cond_t exiting;
#else
	HANDLE	wake_handle;
	HANDLE	exiting;
#endif
};

struct new_thread_args {
	OSthread_func_t *func;
	void *arg;
	struct threadinfo *ti;
};

/* The index is the external thread number modulo the table size; the
 * thread number allocation code makes sure that there are no collisions. */
static struct threadinfo *threadtable[THREADTABLE_SIZE];
#define THREAD(t) (threadtable[(t) % THREADTABLE_SIZE])

/* Number of threads currently in the thread table. */
static long active_threads = 0;

/* Number of threads had beed started in the thread table. */
static long started_threads = 0;

/* Number to use for the next thread created.  The actual number used
 * may be higher than this, in order to avoid collisions in the threadtable.
 * Specifically, (threadnumber % THREADTABLE_SIZE) must be unique for all
 * live threads. */
static long next_threadnumber;

/* Info for the main thread is kept statically, because it should not
 * be deallocated even after the thread module shuts down -- after all,
 * the main thread is still running, and in practice, it can still
 * output debug messages which will require the thread number. */
static struct threadinfo mainthread;

/* Our key for accessing the (struct ssthread *) we stash in the
 * thread-specific-data area.  This is much more efficient than
 * accessing a global table, which we would have to lock. */

/*
 * reference count
 */
static int iInitilizated = 0;

#ifndef WIN32

pthread_key_t tsd_key;
static pthread_mutex_t *threadtable_lock = NULL;
#else
static CRITICAL_SECTION *threadtable_lock = NULL;
#endif

static void lock(void) {
#ifndef WIN32
	int ret;
	if (!threadtable_lock)
		return;

	ret = pthread_mutex_lock(threadtable_lock);

	if (ret != 0) 
	{
		error(ret, "OSthread-pthread: could not lock thread table:%s", strerror(errno));
		//panic(ret, "OSthread-pthread: could not lock thread table");
	}
#else
	if (!threadtable_lock)
		return;

	EnterCriticalSection(threadtable_lock);
#endif
}

static void unlock(void) {
#ifndef WIN32
	int ret;

	if (!threadtable_lock)
		return;

	ret = pthread_mutex_unlock(threadtable_lock);

	if (ret != 0) 
	{
		error(ret, "OSthread-pthread: could not unlock thread table:%s", strerror(errno));
		//panic(ret, "OSthread-pthread: could not unlock thread table");
	}
#else
	if (threadtable_lock)
		LeaveCriticalSection(threadtable_lock);
	return;
#endif
}

/* Empty the wakeup pipe, in case we got several wakeup signals before
 * noticing.  We want to wake up only once. */
static void flushpipe(int fd) {
	unsigned char buf[128];
	size_t bytes;

	do {
#ifndef WIN32
		bytes = read(fd, buf, sizeof(buf));
#else
		bytes = _read(fd, buf, sizeof(buf));
#endif
	} while (bytes > 0);
}

// I copy it from socket.cpp
static int socket_set_blocking(int fd, int blocking)
{
#ifndef WIN32
    int flags, newflags;

    flags = fcntl(fd, F_GETFL);
    if (flags < 0) {
        error(errno, "cannot get flags for fd %d", fd);
        return -1;
    }

    if (blocking)
        newflags = flags & ~O_NONBLOCK;
    else
        newflags = flags | O_NONBLOCK;

    if (newflags != flags) {
        if (fcntl(fd, F_SETFL, newflags) < 0) {
            error(errno, "cannot set flags for fd %d", fd);
            return -1;
        }
    }

    return 0;
#else
	return -1;
#endif
}

/* Allocate and fill a threadinfo structure for a new thread, and store
 * it in a free slot in the thread table.  The thread table must already
 * be locked by the caller.  Return the thread number chosen for this
 * thread.  The caller must make sure that there is room in the table. */
#ifndef WIN32
static long fill_threadinfo(pthread_t id, const char *name,
			    OSthread_func_t *func,
#else
static long fill_threadinfo(long id, const char *name,
			    OSthread_func_t *func,
#endif
struct threadinfo *ti) {
#ifndef WIN32
	int pipefds[2];
	int ret = 0;
#endif
	long first_try;

	OS_assert(active_threads < THREADTABLE_SIZE);

	ti->self = id;
	ti->name = name;
	ti->func = func;

#ifndef WIN32
	if (pipe(pipefds) < 0) 
	{
		error(ret, "cannot allocate wakeup pipe for new thread:%s", strerror(errno));
		//panic(errno, "cannot allocate wakeup pipe for new thread");
	}
	ti->wakefd_recv = pipefds[0];
	ti->wakefd_send = pipefds[1];
	socket_set_blocking(ti->wakefd_recv, 0);
	socket_set_blocking(ti->wakefd_send, 0);

	ret = pthread_cond_init(&ti->exiting, NULL);
	if (ret != 0) 
	{
		error(ret, "cannot create condition variable for new thread:%s", strerror(errno));
//		panic(ret, "cannot create condition variable for new thread");
	}
#else
	ti->exiting		= CreateEvent(NULL, TRUE, FALSE, NULL);
	ti->wake_handle = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (ti->exiting == NULL)
	{
		error(0, "Cannot create new event handle: %s", strerror(errno));
		//panic(0, "Cannot create new event handle.");
	}

	if (ti->wake_handle == NULL)
	{
		error(0, "Cannot create new event handle: %s", strerror(errno));
		//panic(0, "Cannot create new event handle.");
	}
#endif

	/* Find a free table entry and claim it. */
	first_try = next_threadnumber;
	do {
		ti->number = next_threadnumber++;
		/* Check if we looped all the way around the thread table. */
		if (ti->number == first_try + THREADTABLE_SIZE) 
		{
			error(0, "Cannot have more than %d active threads", THREADTABLE_SIZE);
		}
	} while (THREAD(ti->number) != NULL);
	THREAD(ti->number) = ti;

	active_threads++;
	started_threads ++;

	return ti->number;
}

/* Look up the threadinfo pointer for the current thread */
static struct threadinfo *getthreadinfo(void) {
	struct threadinfo *threadinfo = NULL;
#ifndef WIN32
	threadinfo = (struct threadinfo *)pthread_getspecific(tsd_key);
	if (threadinfo == NULL) 
	{
		error(0, "OSthread-pthread: pthread_getspecific failed: %s", strerror(errno));
	} else 
	{
		OS_assert(pthread_equal(threadinfo->self, pthread_self()));
	}
#else
	long iSelf = GetCurrentThreadId();
	int  i = 0;

	/* Find a free table entry and claim it. */
	for (i = 0;i < next_threadnumber; i++)
	{
		if (THREAD(i) == NULL)
			continue;
		if (THREAD(i)->self == iSelf)
			break;
	}

	if (i == next_threadnumber||THREAD(i) == NULL)
		threadinfo = NULL;
	else
		threadinfo = THREAD(i);
#endif
	return threadinfo;
}

static void delete_threadinfo(void) {
	struct threadinfo *threadinfo;
#ifdef WIN32
	long lTempNumber = -1;
#endif

	threadinfo = getthreadinfo();
#ifndef WIN32
	pthread_cond_broadcast(&threadinfo->exiting);
	pthread_cond_destroy(&threadinfo->exiting);
	close(threadinfo->wakefd_recv);
	close(threadinfo->wakefd_send);

	THREAD(threadinfo->number) = NULL;
#else
	SetEvent(threadinfo->exiting);
	CloseHandle(threadinfo->exiting);

	SetEvent(threadinfo->wake_handle);
	CloseHandle(threadinfo->wake_handle);

	lTempNumber = threadinfo->number;
#endif
	active_threads--;
	OS_assert(threadinfo != &mainthread);
	OS_free(threadinfo);
#ifdef WIN32
	THREAD(lTempNumber) = NULL;
#endif
}

static void create_threadinfo_main(void) {
	int ret = 0;

#ifndef WIN32
	fill_threadinfo(pthread_self(), "main", NULL, &mainthread);
	ret = pthread_setspecific(tsd_key, &mainthread);
	if (ret != 0) 
	{
		error(ret, "OSthread-pthread: pthread_getspecific failed: %s", strerror(errno));
	}
#else
	fill_threadinfo(GetCurrentThreadId(), "main", NULL, &mainthread);
#endif
}

// Added by David, 2000-12-5.
void delete_threadinfo_main(void) {
	struct threadinfo *threadinfo;

	threadinfo = getthreadinfo();

	if (!threadinfo) 
		return;
#ifndef WIN32
	pthread_cond_broadcast(&threadinfo->exiting);
	pthread_cond_destroy(&threadinfo->exiting);
	close(threadinfo->wakefd_recv);
	close(threadinfo->wakefd_send);
#else
	SetEvent(threadinfo->exiting);
	CloseHandle(threadinfo->exiting);

	SetEvent(threadinfo->wake_handle);
	CloseHandle(threadinfo->wake_handle);
#endif
	THREAD(threadinfo->number) = NULL;
	active_threads--;
//	OS_assert(threadinfo != &mainthread);
//	OS_free(threadinfo);
}

// initilization function	
void OSThreadInit(void) 
{
	int ret = 0;
	int i;

	if (iInitilizated++)
		return;

#ifndef WIN32
	if (!threadtable_lock)
	{
		threadtable_lock = new pthread_mutex_t;
		if (!threadtable_lock)
			return;
	}

	pthread_mutex_init(threadtable_lock, NULL);
#else
	if (!threadtable_lock)
	{
		threadtable_lock = new CRITICAL_SECTION;
		if (!threadtable_lock)
			return;
	}
	InitializeCriticalSection(threadtable_lock);
#endif

#ifndef WIN32
	ret = pthread_key_create(&tsd_key, NULL);
	if (ret != 0) 
	{
		error(ret, "OSthread-pthread: pthread_key_create failed: %s", strerror(errno));
	}
#else
	//
#endif

	for (i = 0; i < THREADTABLE_SIZE; i++) {
		threadtable[i] = NULL;
	}
	active_threads = 0;

	create_threadinfo_main();

//	OSlib_protected_init();
}

/**
 * Note that the thread library can't shut down completely, because
 * the main thread will still be running, and it may make calls to
 * OSThreadSelf(). 
 */
void OSThreadShutdown(void) 
{
	int ret = 0;
	int running;
	int i;
	static int iShutdowned = 0;

	if (--iInitilizated)
		return;

	/* Main thread must not have disappeared */
	OS_assert(threadtable[0] != NULL);
	lock();
	
	running = 0;
	/* Start i at 1 to skip the main thread, which is supposed to be
	 * still running. 
	 */
	for (i = 1; i < THREADTABLE_SIZE; i++) {
//	for (i = 0; i < THREADTABLE_SIZE; i++) {	// Modified by David, 2000-12-4. I let main thread to exit.
		if (threadtable[i] != NULL) {
			debug("OSlib", 0, "Thread %ld (%s) still running", 
				threadtable[i]->number,
				threadtable[i]->name);
			running++;
		}
	}
	unlock();

	/* We can't do a full cleanup this way */
	if (running)
		return;
#ifndef WIN32
	if (!threadtable_lock)
		return;

	ret = pthread_mutex_destroy(threadtable_lock);
	if (ret != 0) 
	{
		warning(ret, "cannot destroy threadtable lock");
	}

	SAFEDELETE(threadtable_lock);
#else
	if (!threadtable_lock)
		return;

	DeleteCriticalSection(threadtable_lock);
	SAFEDELETE(threadtable_lock);
#endif

	/* We can't delete the tsd_key here, because OSThreadSelf()
	 * still needs it to access the main thread's info. 
	 */


//	// I must delete main thread information in here, matched with create_threadinfo_main() inside of OSthread_init()
	delete_threadinfo_main();	// It must be commented in here, because main thread
								// information will be used in later.
}

#ifdef WIN32
static unsigned * __stdcall new_thread(void *arg)
#else
	static void *new_thread(void *arg)
#endif
{
	int ret = 0;
	struct new_thread_args *p = (struct new_thread_args *)arg;

	/* Make sure we don't start until our parent has entered
	 * our thread info in the thread table. */
	lock();
	unlock();

	/* This has to be done here, because pthread_setspecific cannot
	 * be called by our parent on our behalf.  That's why the ti
	 * pointer is passed in the new_thread_args structure. */
	/* Synchronization is not a problem, because the only thread
	 * that relies on this call having been made is this one --
	 * no other thread can access our TSD anyway. */
#ifndef WIN32
	ret = pthread_setspecific(tsd_key, p->ti);
	if (ret != 0) 
	{
		error(ret, "OSthread-pthread: pthread_setspecific failed: %s", strerror(errno));
	}
#else
	//
#endif
	(p->func)(p->arg);

	lock();
	debug("OSlib.thread", 0, "Thread %ld (%s) terminates.",
		p->ti->number, p->ti->name);
	/* Must free p before signaling our exit, otherwise there is
	 * a race with OS_check_leaks at shutdown. */
	OS_free(p);
	delete_threadinfo();
	unlock();

	return NULL;
}

long OSthread_create_real(OSthread_func_t *func, const char *name, void *arg) 
{
	long ret;
#ifndef WIN32
	pthread_t id;
#else
	long	id = -1;
#endif
	struct new_thread_args *p;
	long number;

	/* We want to pass both these arguments to our wrapper function
	 * new_thread, but the pthread_create interface will only let
	 * us pass one pointer.  So we wrap them in a little struct. */
	p = (struct new_thread_args *)OS_malloc(sizeof(*p));
	p->func = func;
	p->arg = arg;
	p->ti = (struct threadinfo *)OS_malloc(sizeof(*(p->ti)));

	/* Lock the thread table here, so that new_thread can block
	 * on that lock.  That way, the new thread won't start until
	 * we have entered it in the thread table. */
	lock();

	if (active_threads >= THREADTABLE_SIZE) {
		unlock();
		warning(0, "Too many threads, could not create new thread.");
		OS_free(p);
		return -1;
	}

#ifndef WIN32
	ret = pthread_create(&id, NULL, &new_thread, p);
	if (ret != 0) {
		unlock();
		error(ret, "Could not create new thread.");
		OS_free(p);
		return -1;
	}
#else
//	ret = _beginthread((unsigned int (__stdcall *)(void *))func, 0, arg);
	ret = _beginthreadex(NULL, 0, (unsigned (__stdcall *)(void *))new_thread, p, 0, (unsigned *)&id);
//	ret = _beginthreadex(NULL, 0, new_thread, p, 0, (unsigned *)&id);
	if (ret <=0)
	{
		unlock();
		error(ret, "Could not create new thread.");
		OS_free(p);
		return -1;
	}
#endif

#ifndef WIN32
	ret = pthread_detach(id);
	if (ret != 0) {
		warning(ret, "Could not detach new thread.");
	}
#endif

	number = fill_threadinfo(id, name, func, p->ti);
	unlock();

//	printf("Started thread %ld (%s)", number, name);
	
//	debug("OSlib.thread", 0, "Started thread %ld (%s)", 
// 		number, name);

	return number;
}

// wait for single thread to exit, nTimeOut in millionseconds
void OSThreadJoin(long thread, long nTimeOut) 
{
	struct threadinfo *threadinfo;
#ifndef WIN32
	struct timespec abstime;
    	struct timeval now;
	struct timezone zone;
#endif
	int ret = 0;

	if (thread < 0)
		return;

	lock();
	threadinfo = THREAD(thread);
	if (threadinfo == NULL || threadinfo->number != thread) {
		unlock();
		return;
	}

	/* The wait immediately releases the lock, and reacquires it
	 * when the condition is satisfied.  So don't worry, we're not
	 * blocking while keeping the table locked. */
#ifndef WIN32
	if (nTimeOut > 0)
	{
		gettimeofday(&now, &zone);
		abstime.tv_sec = now.tv_sec + nTimeOut / 1000;
		abstime.tv_nsec = now.tv_usec * 1000;
		if (!threadtable_lock)
			return;

		ret = pthread_cond_timedwait(&threadinfo->exiting, threadtable_lock, &abstime);
		if (ret == ETIMEDOUT) 
		{
            /* timeout occurred */
			if (threadinfo->name != NULL)
				warning(0, "OSThreadJoin: thread %s can't stop.", threadinfo->name);

        } else 
		{
                      /* operate on x and y */
        }
	}
	else // nTimeOut <= 0, wait infinitely!
	{
		if (!threadtable_lock)
			return;

		ret = pthread_cond_wait(&threadinfo->exiting, threadtable_lock);
		if (ret != 0) 
		{
			warning(ret, "OSThreadJoin: error in pthread_cond_wait");
		}
	}
	unlock();
#else
	if (nTimeOut <= 0)
		nTimeOut = INFINITE;

	unlock(); // Added here because of using WIN32
	if (threadinfo->number != 0)		// don't wait main thread
		if (WaitForSingleObject(threadinfo->exiting, nTimeOut) == WAIT_TIMEOUT)
		{
			if (threadinfo->name != NULL)
				warning(0, "OSThreadJoin: thread %s can't stop.", threadinfo->name);
		}
//	ResetEvent(threadinfo->exiting);	// pointed by zxm, 2000-11-21
//	unlock(); // Commented because of using WIN32
#endif
}

void OSThreadJoinAll(long nTimeOut) {
	long i;
	long our_thread = OSThreadSelf();

	for (i = 0; i < THREADTABLE_SIZE; ++i) {
		if (THREAD(our_thread) != THREAD(i))
			OSThreadJoin(i, nTimeOut);
	}
}

void OSThreadWakeUpAll(void) {
	long i;
	long our_thread = OSThreadSelf();

	for (i = 0; i < THREADTABLE_SIZE; ++i) {
		if (THREAD(our_thread) != THREAD(i))
			OSThreadWakeUp(i);
	}
}

void OSThreadJoinEvery(OSthread_func_t *func, long nTimeOut) {
	long i;
	struct threadinfo *ti;
	int ret = 0;

	lock();
	for (i = 0; i < THREADTABLE_SIZE; ++i) {
		ti = THREAD(i);
		if (ti == NULL || ti->func != func)
			continue;
		debug("OSlib.thread", 0, 
		    	"Waiting for %ld (%s) to terminate",
			ti->number, ti->name);
#ifndef WIN32
		if (!threadtable_lock)
			return;

		ret = pthread_cond_wait(&ti->exiting, threadtable_lock);
		if (ret != 0) {
			warning(ret, "OSThreadJoin_all: error in "
					"pthread_cond_wait");
		}
#else
		unlock();
		WaitForSingleObject(ti->wake_handle, nTimeOut);
//		ResetEvent(ti->wake_handle);
#endif
	}
	unlock();
}

/* Return the thread id of this thread. */
long OSThreadSelf(void) {
	struct threadinfo *threadinfo = NULL;

#ifndef WIN32
	threadinfo = (struct threadinfo *)pthread_getspecific(tsd_key);
#else
	int i = 0;
	long lCurrentThreadId = GetCurrentThreadId();

	for (i = 0; i < THREADTABLE_SIZE; ++i) 
	{
		if (THREAD(i))
			if (lCurrentThreadId == THREAD(i)->self)
				return i;
	}
#endif
	if (threadinfo)
		return threadinfo->number;
	else
		return -1;
}

void OSThreadWakeUp(long thread) {
	struct threadinfo *threadinfo;
#ifndef WIN32
	unsigned char c = 0;
	int fd;
#endif

	lock();

	threadinfo = THREAD(thread);
	if (threadinfo == NULL || threadinfo->number != thread) {
		unlock();
		return;
	}

#ifndef WIN32
	fd = threadinfo->wakefd_send;
	unlock();
	write(fd, &c, 1);
#else
	unlock();	// Found by Zhang Yuan, 2001-2-19.
	SetEvent(threadinfo->wake_handle);
#endif
}

int OSthread_pollfd(int fd, int events, double timeout) {
#ifndef WIN32
	struct pollfd pollfd[2];
	struct threadinfo *threadinfo;
	int milliseconds;
	int ret;

	threadinfo = getthreadinfo();

	pollfd[0].fd = threadinfo->wakefd_recv;
	pollfd[0].events = POLLIN;

	pollfd[1].fd = fd;
	pollfd[1].events = events;

	milliseconds = (int)(timeout * 1000);

	ret = poll(pollfd, 2, milliseconds);
	if (ret < 0) {
		if (errno != EINTR)
			error(errno, "OSthread_pollfd: error in poll");
		return -1;
	}

	if (pollfd[0].revents)
		flushpipe(pollfd[0].fd);

	return pollfd[1].revents;
#else
	return -1;
#endif
}

int OSthread_poll(struct pollfd *fds, long numfds, double timeout) {
#ifndef WIN32
	struct pollfd *pollfds;
	struct threadinfo *threadinfo;
	int milliseconds;
	int ret;

	threadinfo = getthreadinfo();

	/* Create a new pollfd array with an extra element for the
	 * thread wakeup fd. */

	pollfds = (struct pollfd *)OS_malloc((numfds + 1) * sizeof(*pollfds));
	pollfds[0].fd = threadinfo->wakefd_recv;
	pollfds[0].events = POLLIN;
	memcpy(pollfds + 1, fds, numfds * sizeof(*pollfds));

	milliseconds = (int)(timeout * 1000);

	ret = poll(pollfds, numfds + 1, milliseconds);
	if (ret < 0) {
		if (errno != EINTR)
			error(errno, "OSthread_poll: error in poll");
		return -1;
	}
	if (pollfds[0].revents)
		flushpipe(pollfds[0].fd);

        /* Copy the results back to the caller */
	memcpy(fds, pollfds + 1, numfds * sizeof(*pollfds));
        OS_free(pollfds);

	return ret;
#else
	return -1;
#endif
}

//void OSthread_sleep(double seconds) {
void OSThreadSleep(long nMilliSeconds)
{
#ifndef WIN32
	struct pollfd pollfd;
	int ret;
#endif
	struct threadinfo *threadinfo;

	threadinfo = getthreadinfo();

	if (!threadinfo)
	{
		error(0, "Can not get the current thread information:%s", strerror(errno));
		return;
	}

#ifndef WIN32
	pollfd.fd = threadinfo->wakefd_recv;
	pollfd.events = POLLIN;
#endif

#ifndef WIN32
	ret = poll(&pollfd, 1, nMilliSeconds);
	if (ret < 0) {
		if (errno != EINTR && errno != EAGAIN) {
			warning(errno, "OSthread_sleep: error in poll");
		}
	}
	if (ret == 1) {
		flushpipe(pollfd.fd);
	}
#else
	WaitForSingleObject(threadinfo->wake_handle, nMilliSeconds);
//	ResetEvent(threadinfo->wake_handle);
#endif
}

#ifdef WIN32
/*
 * Added for substitute of OSthread_create. Zhang Yuan, 2000-12-26
 */
#undef FUNC_NAME
#define FUNC_NAME(x) #x
int OSthread_fill_threadinfo(OSthread_func_t *func, long id)
{
	struct threadinfo *ti = NULL;
	long	number = -1;

	ti = (struct threadinfo *)OS_malloc(sizeof(*ti));

	/* Lock the thread table here, so that new_thread can block
	 * on that lock.  That way, the new thread won't start until
	 * we have entered it in the thread table. */
	lock();

	if (active_threads >= THREADTABLE_SIZE) 
	{
		unlock();
		warning(0, "Too many threads, could not create new thread.");
		OS_free(ti);
		return -1;
	}

	number = fill_threadinfo(id, FUNC_NAME(func), func, ti);
	unlock();

	return number;
}

int OSsthread_delete_threadinfo(void)
{
	struct threadinfo *threadinfo;
	long lTempNumber = -1;


	threadinfo = getthreadinfo();

	SetEvent(threadinfo->exiting);
	CloseHandle(threadinfo->exiting);

	SetEvent(threadinfo->wake_handle);
	CloseHandle(threadinfo->wake_handle);

	lTempNumber = threadinfo->number;

	active_threads--;
	OS_assert(threadinfo != &mainthread);
	OS_free(threadinfo);

	THREAD(lTempNumber) = NULL;

	return 0;
}

#endif

/**
 * Return the number of running threads
 */
long OSRunningThreadNumber(void)
{
	return active_threads;
}

/**
 * Return the number of started threads
 */
long OSStartedThreadNumber(void)
{
	return started_threads;
}

/* End of the modification */
