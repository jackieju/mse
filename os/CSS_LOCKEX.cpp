/***************************************************************************
                          CSS_LOCKEX.cpp  -  description
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


#include "CSS_LOCKEX.h"

/**
 * constructor
 */
CSS_SEMAPHORE::CSS_SEMAPHORE()
{
	m_iInit = 0;
}

/**
 * destructor
 */
CSS_SEMAPHORE::~CSS_SEMAPHORE()
{
	if (m_iInit > 0)
		Destroy();
}

/**
 * creator
 */
int CSS_SEMAPHORE::Create(void *pSemaphoreAttr, unsigned int nInit, unsigned int nMax, char *pName)
{
	if (m_iInit ++)
		return LOCKEX_ERR_OK;

#ifndef WIN32
	m_nSema = nInit;
	m_nMax = nMax;
	pthread_mutex_init(&m_mutex,  (pthread_mutexattr_t *)pSemaphoreAttr);		// pthread_mutex_init always return zero
	pthread_mutex_init(&m_cond_mutex,  (pthread_mutexattr_t *)pSemaphoreAttr);
	pthread_cond_init(&m_cond, NULL);						// default initialize
	return LOCKEX_ERR_OK;
#else
	m_pWin32_Sema = ::CreateSemaphore((LPSECURITY_ATTRIBUTES)pSemaphoreAttr,
		nInit, nMax, pName);
	if((int)m_pWin32_Sema == ERROR_ALREADY_EXISTS)
	{
		m_iInit --;
		return LOCKEX_ERR_NAME;
	}
	if(m_pWin32_Sema == NULL)
	{
		m_iInit --;
		return LOCKEX_ERR_ERROR;
	}
	return LOCKEX_ERR_OK;
#endif
}

/**
 * Wait
 */
int CSS_SEMAPHORE::Wait(unsigned int nMillionSeconds)
{
	if (m_iInit <= 0)
		return LOCKEX_ERR_EINVAL;
#ifndef WIN32
	timespec endtime, currtime;
	timeb timenow;

	if(nMillionSeconds != INFINITE)
	{
		// get current time
		ftime(&timenow);
		currtime.tv_sec = timenow.time;
		currtime.tv_nsec = LOCKEX_NANOSEC_PER_MILLISEC*timenow.millitm;
		// caculate end time
		endtime.tv_sec = timenow.time+nMillionSeconds/1000+(timenow.millitm+nMillionSeconds%1000)/1000;
		endtime.tv_nsec = LOCKEX_NANOSEC_PER_MILLISEC*((timenow.millitm+nMillionSeconds%1000)%1000);
	}
	
	int nret;
	while(true)
	{
		pthread_mutex_lock(&m_mutex);
		if(m_nSema > 0)
		{
			m_nSema--;
			pthread_mutex_unlock(&m_mutex);
			return LOCKEX_ERR_OK;
		}
		pthread_mutex_unlock(&m_mutex);

		pthread_mutex_lock(&m_cond_mutex);
		if(nMillionSeconds != INFINITE)
			nret = pthread_cond_timedwait(&m_cond, &m_cond_mutex, &endtime);
		else
			nret = pthread_cond_wait(&m_cond, &m_cond_mutex);			// wait forever
		pthread_mutex_unlock(&m_cond_mutex);
		if(nret == 0)
			continue;
		else if(nret == ETIMEDOUT)
			return LOCKEX_ERR_TIMEOUT;
		else
			return LOCKEX_ERR_ERROR;
	}
#else
	int nret = ::WaitForSingleObject(m_pWin32_Sema, nMillionSeconds);
	if(nret == WAIT_OBJECT_0)
		return LOCKEX_ERR_OK;
	else if(nret == WAIT_TIMEOUT)
		return LOCKEX_ERR_TIMEOUT;
	else
		return LOCKEX_ERR_ERROR;
#endif
}

/**
 * Release
 */
int CSS_SEMAPHORE::Release(int nCount)
{
	if (m_iInit <= 0)
		return LOCKEX_ERR_EINVAL;
	if(nCount <= 0)
		return LOCKEX_ERR_NUMBER;
#ifndef WIN32
	pthread_mutex_lock(&m_mutex);
	m_nSema += nCount;
	m_nSema = (m_nMax > m_nSema) ? m_nSema : m_nMax;
	pthread_mutex_unlock(&m_mutex);
	
	pthread_mutex_lock(&m_cond_mutex);
	int nret = pthread_cond_broadcast(&m_cond);
	pthread_mutex_unlock(&m_cond_mutex);
	if(nret == 0)
		return LOCKEX_ERR_OK;
	else
		return LOCKEX_ERR_ERROR;
#else
	int nret = ::ReleaseSemaphore(m_pWin32_Sema, nCount, NULL);
	if(nret == 0)
		return LOCKEX_ERR_ERROR;
	else
		return LOCKEX_ERR_OK;
#endif
}

/**
 * Destroy
 */
int CSS_SEMAPHORE::Destroy()
{
	if (-- m_iInit)
		return LOCKEX_ERR_OK;
#ifndef WIN32
	int nreturn = LOCKEX_ERR_OK;
	int nret = pthread_mutex_destroy(&m_mutex);
	if(nret == EBUSY)
		nreturn = LOCKEX_ERR_BUSY;
	else if(nret != 0)
		nreturn = LOCKEX_ERR_ERROR;
	nret = pthread_mutex_destroy(&m_cond_mutex);
	if(nret == EBUSY)
		nreturn = LOCKEX_ERR_BUSY;
	else if(nret != 0)
		nreturn = LOCKEX_ERR_ERROR;
	nret = pthread_cond_destroy(&m_cond);
	if(nret == EBUSY)
		nreturn = LOCKEX_ERR_BUSY;
	else if(nret != 0)
		nreturn = LOCKEX_ERR_ERROR;

	return nreturn;
#else
	if(::CloseHandle(m_pWin32_Sema))
		return LOCKEX_ERR_OK;
	else
		return LOCKEX_ERR_ERROR;
#endif
}

/**
 * constructor
 */
CSS_MUTEX::CSS_MUTEX()
{
	m_iInit = 0;
}

/**
 * destructor
 */
CSS_MUTEX::~CSS_MUTEX()
{
	if (m_iInit > 0)
		Destroy();
}

/**
 * Create
 */
int CSS_MUTEX::Create(void *pMutexAttr)
{
	if (m_iInit ++)
		return LOCKEX_ERR_OK;
#ifndef WIN32
	pthread_mutex_init(&m_mutex,  (pthread_mutexattr_t *)pMutexAttr);		// pthread_mutex_init always return zero	
	return LOCKEX_ERR_OK;
#else
	::InitializeCriticalSection(&m_Win32_Mutex);
	return LOCKEX_ERR_OK;
#endif
}

/**
 * Lock
 */
int CSS_MUTEX::Lock()
{
	if (m_iInit <= 0)
		return LOCKEX_ERR_EINVAL;
#ifndef WIN32
	int nret = pthread_mutex_lock(&m_mutex);
	if(nret == 0)
		return LOCKEX_ERR_OK;
	else if(nret == EINVAL)
		return LOCKEX_ERR_EINVAL;
	else if(nret == EDEADLK)
		return LOCKEX_ERR_EDEADLK;
	else
		return LOCKEX_ERR_ERROR;
#else
	::EnterCriticalSection(&m_Win32_Mutex);
	return LOCKEX_ERR_OK;
#endif
}

/**
 * Unlock
 */
int CSS_MUTEX::Unlock()
{
	if (m_iInit <= 0)
		return LOCKEX_ERR_EINVAL;
#ifndef WIN32
	int nret = pthread_mutex_unlock(&m_mutex);
	if(nret == 0)
		return LOCKEX_ERR_OK;
	else if(nret == EINVAL)
		return LOCKEX_ERR_EINVAL;
	else if(nret == EDEADLK)
		return LOCKEX_ERR_EPERM;
	else
		return LOCKEX_ERR_ERROR;
#else
	::LeaveCriticalSection(&m_Win32_Mutex);
	return LOCKEX_ERR_OK;
#endif
}

/**
 * Destroy
 */
int CSS_MUTEX::Destroy()
{
	if (-- m_iInit)
		return LOCKEX_ERR_OK;
#ifndef WIN32
	int nret = pthread_mutex_destroy(&m_mutex);
	if(nret == EBUSY)	
		return LOCKEX_ERR_BUSY;
	else if(nret != 0)
		return LOCKEX_ERR_ERROR;
	else
		return LOCKEX_ERR_OK;
#else
	::DeleteCriticalSection(&m_Win32_Mutex);
	return LOCKEX_ERR_OK;
#endif
}

/**
 * constructor
 */
CSS_EVENT::CSS_EVENT()
{
	m_iInit = 0;
}

/**
 * destructor
 */
CSS_EVENT::~CSS_EVENT()
{
	if (m_iInit > 0)
		Destroy();
}

/**
 * Create
 */
int CSS_EVENT::Create(void *pEventAttr, bool bManualReset, bool bInitState, char *pName)
{
	if (m_iInit ++)
		return LOCKEX_ERR_OK;

	m_bManualReset = bManualReset;
	m_nWait = 0;
#ifndef WIN32
	pthread_mutex_init(&m_mutex,  (pthread_mutexattr_t *)pEventAttr);		// pthread_mutex_init always return zero
	pthread_mutex_init(&m_cond_mutex,  (pthread_mutexattr_t *)pEventAttr);
	pthread_cond_init(&m_cond, NULL);;						// default initialize
	if(bInitState)
		m_nSignal = LOCKEX_MAXSET;
	else
		m_nSignal = 0;
	return LOCKEX_ERR_OK;
#else
	m_pWin32_Event = ::CreateEvent((LPSECURITY_ATTRIBUTES)pEventAttr,
		bManualReset, bInitState, pName);
	::InitializeCriticalSection(&m_Win32_Mutex);
	if((int)m_pWin32_Event == ERROR_ALREADY_EXISTS)
	{
		m_iInit --;
		return LOCKEX_ERR_NAME;
	}
	else if(m_pWin32_Event == NULL)
	{
		m_iInit --;
		return LOCKEX_ERR_ERROR;
	}
	else
		return LOCKEX_ERR_OK;
#endif
}

/**
 * Wait
 */
int CSS_EVENT::Wait(unsigned int nMillionSeconds)
{
	if (m_iInit <= 0)
		return LOCKEX_ERR_EINVAL;
#ifndef WIN32
	timespec endtime, currtime;
	timeb timenow;
	
	if(nMillionSeconds != INFINITE)
	{
		// get current time
		ftime(&timenow);	
		currtime.tv_sec = timenow.time;
		currtime.tv_nsec = LOCKEX_NANOSEC_PER_MILLISEC*timenow.millitm;
		// caculate end time
		endtime.tv_sec = timenow.time+nMillionSeconds/1000+(timenow.millitm+nMillionSeconds%1000)/1000;
		endtime.tv_nsec = LOCKEX_NANOSEC_PER_MILLISEC*((timenow.millitm+nMillionSeconds%1000)%1000);
	}
	
	int nret;
	bool bUserCountAdded = false;
	while(true)
	{
		pthread_mutex_lock(&m_mutex);
		if(!bUserCountAdded)
		{
			bUserCountAdded = true;
			m_nWait++;
		}
		if(m_nSignal > 0)
		{
			m_nWait--;
			if(!m_bManualReset)
			{
				m_nSignal--;
				if (m_nWait <= 0)
					m_nSignal = m_nWait = 0;
			}
			pthread_mutex_unlock(&m_mutex);
			return LOCKEX_ERR_OK;
		}
		pthread_mutex_unlock(&m_mutex);
		
		pthread_mutex_lock(&m_cond_mutex);
		if(nMillionSeconds != INFINITE)
			nret = pthread_cond_timedwait(&m_cond, &m_cond_mutex, &endtime);
		else
			nret = pthread_cond_wait(&m_cond, &m_cond_mutex);			// wait forever
		pthread_mutex_unlock(&m_cond_mutex);
		if(nret == 0)
			continue;
		else if(nret == ETIMEDOUT)
			return LOCKEX_ERR_TIMEOUT;
		else
			return LOCKEX_ERR_ERROR;
	}
#else
	::EnterCriticalSection(&m_Win32_Mutex);
	m_nWait++;
	::LeaveCriticalSection(&m_Win32_Mutex);

	int nret = ::WaitForSingleObject(m_pWin32_Event, nMillionSeconds);

	::EnterCriticalSection(&m_Win32_Mutex);
	m_nWait--;
	::LeaveCriticalSection(&m_Win32_Mutex);

	if(nret == WAIT_OBJECT_0)
		return LOCKEX_ERR_OK;
	else if(nret == WAIT_TIMEOUT)
		return LOCKEX_ERR_TIMEOUT;
	else
		return LOCKEX_ERR_ERROR;
#endif
}

/**
 * Set
 */
int CSS_EVENT::Set()
{
	if (m_iInit <= 0)
		return LOCKEX_ERR_EINVAL;
#ifndef WIN32
	pthread_mutex_lock(&m_mutex);
	if(m_bManualReset)
		m_nSignal = LOCKEX_MAXSET;
	else
	{
		m_nSignal++;
	}
	pthread_mutex_unlock(&m_mutex);
	
	pthread_mutex_lock(&m_cond_mutex);
	if(m_bManualReset)
		pthread_cond_broadcast(&m_cond);
	else
		pthread_cond_signal(&m_cond);
	pthread_mutex_unlock(&m_cond_mutex);
	return LOCKEX_ERR_OK;
#else
	if(::SetEvent(m_pWin32_Event))
		return LOCKEX_ERR_OK;
	else
		return LOCKEX_ERR_ERROR;
#endif
}

/**
 * Reset
 */
int CSS_EVENT::Reset()
{
	if (m_iInit <= 0)
		return LOCKEX_ERR_EINVAL;
#ifndef WIN32
	pthread_mutex_lock(&m_mutex);
	m_nSignal = 0;
	pthread_mutex_unlock(&m_mutex);
	return LOCKEX_ERR_OK;
#else
	if(::ResetEvent(m_pWin32_Event))
		return LOCKEX_ERR_OK;
	else
		return LOCKEX_ERR_ERROR;
#endif
}

/**
 * Broadcast
 */
int CSS_EVENT::Broadcast()
{
	if (m_iInit <= 0)
		return LOCKEX_ERR_EINVAL;
#ifndef WIN32
	pthread_mutex_lock(&m_cond_mutex);
	pthread_cond_broadcast(&m_cond);
	pthread_mutex_unlock(&m_cond_mutex);
	return LOCKEX_ERR_OK;
#else
	::EnterCriticalSection(&m_Win32_Mutex);
	for(int nI=0; nI < m_nWait; nI++)
	{
		if(!::SetEvent(m_pWin32_Event))
		{
			::LeaveCriticalSection(&m_Win32_Mutex);
			return LOCKEX_ERR_ERROR;
		}
	}
	::LeaveCriticalSection(&m_Win32_Mutex);
	return LOCKEX_ERR_OK;
#endif
}

/**
 * Destroy
 */
int CSS_EVENT::Destroy()
{
	if (-- m_iInit)
		return LOCKEX_ERR_OK;
#ifndef WIN32
	int nreturn = LOCKEX_ERR_OK;
	int nret = pthread_mutex_destroy(&m_mutex);
	if(nret == EBUSY)	
		nreturn = LOCKEX_ERR_BUSY;
	else if(nret != 0)
		nreturn = LOCKEX_ERR_ERROR;
	nret = pthread_mutex_destroy(&m_cond_mutex);
	if(nret == EBUSY)	
		nreturn = LOCKEX_ERR_BUSY;
	else if(nret != 0)
		nreturn = LOCKEX_ERR_ERROR;
	nret = pthread_cond_destroy(&m_cond);
	if(nret == EBUSY)	
		nreturn = LOCKEX_ERR_BUSY;
	else if(nret != 0)
		nreturn = LOCKEX_ERR_ERROR;

	return nreturn;
#else
	::EnterCriticalSection(&m_Win32_Mutex);
	if (m_nWait > 0)
	{
		::LeaveCriticalSection(&m_Win32_Mutex);
		return LOCKEX_ERR_BUSY;
	}
	if(!::CloseHandle(m_pWin32_Event))
	{
		::LeaveCriticalSection(&m_Win32_Mutex);
		return LOCKEX_ERR_ERROR;
	}
	::LeaveCriticalSection(&m_Win32_Mutex);
	::DeleteCriticalSection(&m_Win32_Mutex);

	return LOCKEX_ERR_OK;
#endif	
}

/**
 * constructor
 */
CSS_SingleWriteMultiRead::CSS_SingleWriteMultiRead()
{
	m_iInit = 0;
}

/**
 * destructor
 */
CSS_SingleWriteMultiRead::~CSS_SingleWriteMultiRead()
{
	if (m_iInit > 0)
		Destroy();
}

/**
 * Create
 */
int CSS_SingleWriteMultiRead::Create(int nMaxReaders)
{
	if (m_iInit ++)
		return LOCKEX_ERR_OK;
#ifndef WIN32
	m_nReading = 0;
	m_bWriting = false;
	pthread_mutex_init(&m_mutex,  NULL);					// pthread_mutex_init always return zero
	pthread_mutex_init(&m_cond_mutex,  NULL);
	pthread_cond_init(&m_cond, NULL);						// default initialize
	return LOCKEX_ERR_OK;
#else
	m_hMutexNoWriter = CreateMutex(NULL, FALSE, NULL);

	m_hEventNoReaders = CreateEvent(NULL, TRUE, TRUE, NULL);

	m_hSemNumReaders = CreateSemaphore(NULL, 0, nMaxReaders, NULL);

	if ((NULL == m_hMutexNoWriter)	||
		(NULL == m_hEventNoReaders)	||
		(NULL == m_hSemNumReaders) ){
//		TRACE("No enough system resource for create SRMW Object\r\n");
		m_iInit --;
		return LOCKEX_ERR_ERROR;
	}
	else
		return LOCKEX_ERR_OK;
	//ASSERT(m_hMutexNoWriter && m_hEventNoReaders && m_hSemNumReaders);
#endif
}

/**
 * WaitToWrite
 */
int CSS_SingleWriteMultiRead::WaitToWrite(unsigned int nMillionSeconds)
{
	if (m_iInit <= 0)
		return LOCKEX_ERR_EINVAL;
#ifndef WIN32
	timespec endtime, currtime;
	timeb timenow;

	if(nMillionSeconds != INFINITE)
	{
		// get current time
		ftime(&timenow);
		currtime.tv_sec = timenow.time;
		currtime.tv_nsec = LOCKEX_NANOSEC_PER_MILLISEC*timenow.millitm;
		// caculate end time
		endtime.tv_sec = timenow.time+nMillionSeconds/1000+(timenow.millitm+nMillionSeconds%1000)/1000;
		endtime.tv_nsec = LOCKEX_NANOSEC_PER_MILLISEC*((timenow.millitm+nMillionSeconds%1000)%1000);
	}

	int nret;
	while(true)
	{
		pthread_mutex_lock(&m_mutex);
		if(!m_bWriting && m_nReading == 0)							// you can write only when no reading and writing
		{
			m_bWriting = true;
			pthread_mutex_unlock(&m_mutex);
			return LOCKEX_ERR_OK;
		}
		pthread_mutex_unlock(&m_mutex);

		pthread_mutex_lock(&m_cond_mutex);
		if(nMillionSeconds != INFINITE)
			nret = pthread_cond_timedwait(&m_cond, &m_cond_mutex, &endtime);
		else
			nret = pthread_cond_wait(&m_cond, &m_cond_mutex);			// wait forever
		pthread_mutex_unlock(&m_cond_mutex);
		if(nret == 0)
			continue;
		else if(nret == ETIMEDOUT)
			return LOCKEX_ERR_TIMEOUT;
		else
			return LOCKEX_ERR_ERROR;
	}
#else
	OS_UL dw;
	HANDLE aHandles[2];

	aHandles[0] = m_hMutexNoWriter;
	aHandles[1] = m_hEventNoReaders;

	dw = WaitForMultipleObjects(2, aHandles, TRUE, nMillionSeconds);

	if (dw == WAIT_TIMEOUT)
		return LOCKEX_ERR_TIMEOUT;
	else if(dw == WAIT_OBJECT_0)
		return LOCKEX_ERR_OK;
	else
		return LOCKEX_ERR_ERROR;
#endif
}

/**
 * DoneWriting
 */
int CSS_SingleWriteMultiRead::DoneWriting()
{
	if (m_iInit <= 0)
		return LOCKEX_ERR_EINVAL;
#ifndef WIN32
	pthread_mutex_lock(&m_mutex);
	m_bWriting = false;
	pthread_mutex_lock(&m_cond_mutex);
	pthread_cond_broadcast(&m_cond);
	pthread_mutex_unlock(&m_cond_mutex);
	pthread_mutex_unlock(&m_mutex);
	return LOCKEX_ERR_ERROR;
#else
	ReleaseMutex(m_hMutexNoWriter);
	return LOCKEX_ERR_OK;
#endif
}

/**
 * WaitToRead
 */
int CSS_SingleWriteMultiRead::WaitToRead(unsigned int nMillionSeconds)
{
	if (m_iInit <= 0)
		return LOCKEX_ERR_EINVAL;
#ifndef WIN32
	timespec endtime, currtime;
	timeb timenow;

	if(nMillionSeconds != INFINITE)
	{
		// get current time
		ftime(&timenow);
		currtime.tv_sec = timenow.time;
		currtime.tv_nsec = LOCKEX_NANOSEC_PER_MILLISEC*timenow.millitm;
		// caculate end time
		endtime.tv_sec = timenow.time+nMillionSeconds/1000+(timenow.millitm+nMillionSeconds%1000)/1000;
		endtime.tv_nsec = LOCKEX_NANOSEC_PER_MILLISEC*((timenow.millitm+nMillionSeconds%1000)%1000);
	}

	int nret;
	while(true)
	{
		pthread_mutex_lock(&m_mutex);
		if(!m_bWriting)											// you can read when no one is writing
		{
			m_nReading++;
			pthread_mutex_unlock(&m_mutex);
			return LOCKEX_ERR_OK;
		}
		pthread_mutex_unlock(&m_mutex);

		pthread_mutex_lock(&m_cond_mutex);
		if(nMillionSeconds != INFINITE)
			nret = pthread_cond_timedwait(&m_cond, &m_cond_mutex, &endtime);
		else
			nret = pthread_cond_wait(&m_cond, &m_cond_mutex);			// wait forever
		pthread_mutex_unlock(&m_cond_mutex);
		if(nret == 0)
			continue;
		else if(nret == ETIMEDOUT)
			return LOCKEX_ERR_TIMEOUT;
		else
			return LOCKEX_ERR_ERROR;
	}
#else
	OS_UL dw;
	LONG lPreviousCount;

	dw = WaitForSingleObject(m_hMutexNoWriter, nMillionSeconds);

	if (dw != WAIT_TIMEOUT){
		// This thread can read from the shared data.
		// Increment the number of reader threads
		ReleaseSemaphore(m_hSemNumReaders, 1, &lPreviousCount);

		if (lPreviousCount == 0){
			// if this is the first reader thread
			// set the evnet to reflect this
			ResetEvent(m_hEventNoReaders);
		}
		// Allow other writer/reader threads to use
		// the synchronization object.
		ReleaseMutex(m_hMutexNoWriter);
	}

	if(dw == WAIT_OBJECT_0)
		return LOCKEX_ERR_OK;
	else if(dw == WAIT_TIMEOUT)
		return LOCKEX_ERR_TIMEOUT;
	else
		return LOCKEX_ERR_ERROR;
#endif
}

/**
 * DoneReading
 */
int CSS_SingleWriteMultiRead::DoneReading()
{
	if (m_iInit <= 0)
		return LOCKEX_ERR_EINVAL;
#ifndef WIN32
	pthread_mutex_lock(&m_mutex);
	m_nReading--;
	if(m_nReading <= 0)
	{
		m_nReading = 0;
		pthread_mutex_lock(&m_cond_mutex);
		pthread_cond_broadcast(&m_cond);
		pthread_mutex_unlock(&m_cond_mutex);
	}
	pthread_mutex_unlock(&m_mutex);
	return LOCKEX_ERR_ERROR;
#else
	BOOL fLastReader;

	HANDLE aHandles[2];

	// We can stop reading if the mutex guard is available.
	// but when we stop reading we must also decrement the
	// number of reader threads.
	aHandles[0] = m_hMutexNoWriter;
	aHandles[1] = m_hSemNumReaders;

	WaitForMultipleObjects(2, aHandles, TRUE, INFINITE);

	fLastReader = (WaitForSingleObject(m_hSemNumReaders, 0) == WAIT_TIMEOUT);

	if (fLastReader){
		// if this is the last reader thread
		// set the event to reflect this.
		SetEvent(m_hEventNoReaders);
	}else{
		// if this is not hte last reader theread, we successfully
		// waited on the semaphore. We must release the semaphore
		// so that the count accurately reflects the number
		// of reader threads
		ReleaseSemaphore(m_hSemNumReaders, 1, NULL);
	}

	ReleaseMutex(m_hMutexNoWriter);
	return LOCKEX_ERR_OK;
#endif
}

/**
 * Destroy
 */
int CSS_SingleWriteMultiRead::Destroy()
{
	if (-- m_iInit)
		return LOCKEX_ERR_OK;
#ifndef WIN32
	int nreturn = LOCKEX_ERR_OK;
	int nret = pthread_mutex_destroy(&m_mutex);
	if(nret == EBUSY)
		nreturn = LOCKEX_ERR_BUSY;
	else if(nret != 0)
		nreturn = LOCKEX_ERR_ERROR;
	nret = pthread_mutex_destroy(&m_cond_mutex);
	if(nret == EBUSY)
		nreturn = LOCKEX_ERR_BUSY;
	else if(nret != 0)
		nreturn = LOCKEX_ERR_ERROR;
	nret = pthread_cond_destroy(&m_cond);
	if(nret == EBUSY)
		nreturn = LOCKEX_ERR_BUSY;
	else if(nret != 0)
		nreturn = LOCKEX_ERR_ERROR;

	return nreturn;
#else
	if (m_hMutexNoWriter)
		CloseHandle(m_hMutexNoWriter);
	if (m_hEventNoReaders)
		CloseHandle(m_hEventNoReaders);
	if (m_hSemNumReaders)
		CloseHandle(m_hSemNumReaders);
	return LOCKEX_ERR_OK;
#endif
}
