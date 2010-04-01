/**
 * OSthread.h - threads wrapper with interruptible sleep and poll operations.
 * @author: Zhang Yuan
 * @package OSLib
 * @version 1.0.0
 * @Date  : 2001-5-28
 * This is a (partial) encapsulation of threads.  It provides functions
 * to create new threads and to manipulate threads.  It will eventually
 * be extended to encapsulate all pthread functions we use, so that
 * non-POSIX platforms can plug in their own versions.
 */

#ifndef __OSTHREAD_H__
#define __OSTHREAD_H__

#include "osmacros.h"

/* OSthread_self() must return this value for the main thread. */
#define MAIN_THREAD_ID 0

typedef void OSthread_func_t(void *arg);

/**
 * <pre>
 * Thread library initilization. Called by the OSlib init code.
 * Parameters:
 *				None
 * Return values:
 *				None
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD void OSThreadInit(void);

/**
 * <pre>
 * Thread library destruction. Called by the OSlib init code.
 * Parameters:
 *				None
 * Return values:
 *				None
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD void OSThreadShutdown(void);

/**
 * <pre>
 * Start a new thread, running func(arg).  Return the new thread ID
 * on success, or -1 on failure.  Thread IDs are unique during the lifetime
 * of the entire process, unless you use more than LONG_MAX threads.
 * Parameters:
 *				None
 * Return values:
 *				<0 : failure
 *				>0 : thread ID
 * </pre>
 */
#define OSThreadCreate(func, arg) \
	(OSthread_create_real(func, __FILE__ ":" #func, arg))
OSLIB_DLL_FUNC_HEAD long OSthread_create_real(OSthread_func_t *func, const char *funcname,
			  void *arg);

/**
 * <pre>
 * Wait for the other thread to terminate.  Return immediately if it
 * has already terminated.
 * Parameters:
 *				thread, thread id 
 *				nTimeOut, waiting timeout
 * Return values:
 *				None
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD void OSThreadJoin(long thread, long nTimeOut = 0);

/**
 * <pre>
 * Wait for all threads whose main function is `func' to terminate.
 * Return immediately if none are running.
 * Parameters:
 *				func, thread function 
 *				nTimeOut, waiting timeout
 * Return values:
 *				None
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD void OSThreadJoinEvery(OSthread_func_t *func, long nTimeOut = 0);

/**
 * <pre>
 * Wait for all threads to terminate.  Return immediately if none
 * are running.  This function is not intended to be called if new
 * threads are still being created, and it may not notice such threads.
 * Parameters:
 *				nTimeOut, waiting timeout
 * Return values:
 *				None
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD void OSThreadJoinAll(long nTimeOut = 0);

/**
 * <pre>
 * Return the thread id of this thread.  Note that it may be called for
 * the main thread even before the thread library has been initialized
 * and after it had been shut down.
 * Parameters:
 *				None
 * Return values:
 *				current thread id
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD long OSThreadSelf(void);

/**
 * <pre>
 * If the other thread is currently in OSthread_pollfd or OSthread_sleep,
 * make it return immediately.  Otherwise, make it return immediately, the
 * next time it calls one of those functions.
 * Parameters:
 *				thread, thread id
 * Return values:
 *				None
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD void OSThreadWakeUp(long thread);

/**
 * <pre>
 * Wake up all threads
 * Parameters:
 *				None
 * Return values:
 *				None
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD void OSThreadWakeUpAll(void);

/**
 * <pre>
 * Sleep until "seconds" seconds have elapsed, or until another thread
 * calls OSthread_wakeup on us.  Fractional seconds are allowed.
 * Parameters:
 *				seconds, sleeping time in seconds
 * Return values:
 *				None
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD void OSThreadSleep(long nMillSeconds = 100);

/**
 * <pre>
 * Delete the information about main thread.
 * Parameters:
 *				None
 * Return values:
 *				None
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD void delete_threadinfo_main(void);

/**
 * <pre>
 * Return the number of running threads.
 * Parameters:
 *				None
 * Return values:
 *				the number of total running threads
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD long OSRunningThreadNumber(void);

/**
 * <pre>
 * Return the number of started threads.
 * Parameters:
 *				None
 * Return values:
 *				the number of total started threads
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD long OSStartedThreadNumber(void);

#ifdef WIN32
/**
 * <pre>
 * Fill the information of one thread, for private use.
 * Parameters:
 *				func, function name
 *				id, thread id
 * Return values:
 *				<0 : failure
 *				>0 : success
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSthread_fill_threadinfo(OSthread_func_t *func, long id);

/**
 * <pre>
 * Delete the information of current thread, for private use.
 * Parameters:
 *				None
 * Return values:
 *				<0 : failure
 *				>0 : success
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSthread_delete_threadinfo(void);

///**
// * <pre>
// * Wait for a funtction to exit.
// * Parameters:
// *				func, the function address (name)
// * Return values:
// *				None
// * </pre>
// */
//void SSThreadJoinEvery(OSthread_func_t *func);

#endif

#endif
