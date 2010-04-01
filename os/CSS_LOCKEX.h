//: CSS_LOCKEX.h
/**
 * @author  xfwang
 * @package SSLib
 * @version 1.0.0
 */

/***************************************************************************
                          CSS_LOCKEX.h  -  description
                             -------------------
    begin                : Fri Jan 4 2002
    copyright            : (C) 2002 by xfwang
    email                : xfwang@js-xfwang-01
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _CSS_LOCKEX_H_
#define _CSS_LOCKEX_H_

#ifndef WIN32
	#include <unistd.h>
	#include <errno.h>
	#include <pthread.h>
	#include <sys/time.h>
	#include <sys/timeb.h>
    #include <unistd.h>
	#include <sys/poll.h>
#else
	#include <windows.h>
	#include <process.h>
	#include <io.h>
#endif

#include "osmacros.h"

#ifndef INFINITE
#define INFINITE					0xFFFFFFFF
#endif

#define LOCKEX_NANOSEC_PER_MILLISEC			1000000
#define LOCKEX_MAXSET				0x0FFFFFFF
////////////////////////////////////////////////////////////////////////////////
//			ERROR CODE define
////////////////////////////////////////////////////////////////////////////////
//#ifndef WIN32
//#else
#define LOCKEX_ERR_TIMEOUT			1						// timeout
#define LOCKEX_ERR_OK				0						// success
#define LOCKEX_ERR_NAME				-1						// user specified name has exist in system 		
#define LOCKEX_ERR_NUMBER			-2						// user specified nubmer is invaliad
#define LOCKEX_ERR_BUSY				-3						// user want to destroy when another thread is waiting
#define LOCKEX_ERR_EINVAL			-4						// the mutex has not been properly initialized
#define LOCKEX_ERR_EDEADLK			-5						// the mutex is already locked by  the  calling thread (``error checking'' mutexes only)
#define LOCKEX_ERR_EPERM			-6						// the calling thread does not  own  the  mutex (``error checking'' mutexes only)
#define LOCKEX_ERR_ERROR			-100					// error occured, you can
//#endif


/**
 * semaphore
 */
class OSLIB_DLL_FUNC_HEAD CSS_SEMAPHORE
{
private:
#ifndef WIN32
	int m_nSema;							// internal counter for semaphore
	int m_nMax;					// internal max-value for semphore
	pthread_mutex_t m_mutex;				// this mutex is for 'm_nSema'
	pthread_mutex_t m_cond_mutex;			// this mutex is for 'm_cond_mutex'
	pthread_cond_t m_cond;
#else
	HANDLE m_pWin32_Sema;					// Under windows op_system, semaphore is a sys-handle
#endif
	int m_iInit;

public:

	CSS_SEMAPHORE();
	~CSS_SEMAPHORE();

	int Create(void *pSemaphoreAttr = NULL, unsigned int nInit = 0, unsigned int nMax = 65535, char *pName = NULL);
	int Release(int nCount=1);
	int Wait(unsigned int nMillionSeconds=INFINITE);
	int Destroy();
};

/**
 * mutex
 */
class OSLIB_DLL_FUNC_HEAD CSS_MUTEX
{
private:
#ifndef WIN32
	pthread_mutex_t m_mutex;				// Under unix op_system, mutex is pthread_mutex_t
#else
	CRITICAL_SECTION m_Win32_Mutex;			// Under windows op_system, mutex is critialsection
#endif
	int m_iInit;
public:
	CSS_MUTEX();
	~CSS_MUTEX();

	int Create(void *pMutexAttr);
	int Unlock();
	int Lock();
	int Destroy();
};

/**
 * event
 */
class OSLIB_DLL_FUNC_HEAD CSS_EVENT
{
private:
	int m_nWait;							// counter for waiting
	bool m_bManualReset;
#ifndef WIN32
	pthread_mutex_t m_mutex;				// mutex for m_nWait
	pthread_mutex_t m_cond_mutex;
	pthread_cond_t m_cond;
	int m_nSignal;							// keep signal for multi_set
#else
	HANDLE m_pWin32_Event;
	CRITICAL_SECTION m_Win32_Mutex;			// criticalsection for m_nWait
#endif
	int m_iInit;

public:
	CSS_EVENT();
	~CSS_EVENT();

	int Create(void *pEventAttr, bool bManualReset, bool bInitState, char* pName);
	int Set();
	int Reset();
	int Broadcast();						// release all waiters
	int Wait(unsigned int nMillionSeconds=INFINITE);
	int Destroy();
};

/**
 * CSS_SingleWriteMultiRead
 */
class OSLIB_DLL_FUNC_HEAD CSS_SingleWriteMultiRead
{
private:
#ifndef WIN32
	int m_nReading;						// counter for reading
	bool m_bWriting;						// mark of writing
	pthread_mutex_t m_mutex;
	pthread_mutex_t m_cond_mutex;
	pthread_cond_t m_cond;
#else
	HANDLE m_hMutexNoWriter;
	HANDLE m_hEventNoReaders;
	HANDLE m_hSemNumReaders;
#endif
	int m_iInit;

public:
	CSS_SingleWriteMultiRead();
	~CSS_SingleWriteMultiRead();

	int Create(int nMaxReaders = 0x7FFFFFFF);
	int WaitToWrite(unsigned int  nMillionSeconds= INFINITE);
	int DoneWriting();
	int WaitToRead(unsigned int nMillionSeconds = INFINITE);
	int DoneReading();
	int Destroy();
};

#endif
//:~

