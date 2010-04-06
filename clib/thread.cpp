#include "thread.h"

#ifdef WIN32
#include "process.h"
#else
#include  <sys/time.h>
#endif

using namespace JUJU;

#ifndef WIN32

pthread_key_t tsd_key1;
#else
#endif

struct new_thread_args {
	thread_func_t *func;
	void *arg;
	struct threadinfo *ti;
};
struct threadinfo {
#ifndef WIN32
	pthread_t self;
#else
	long self;
#endif
	const char *name;
	thread_func_t *func;
	long number;

#ifndef WIN32
	int wakefd_recv;
	int wakefd_send;
	pthread_cond_t exiting;
#else

#endif
};

long g_nThreadNum = 0;
#ifdef WIN32
static void * __stdcall new_thread(void *arg)
#else
	static void *new_thread(void *arg)
#endif
{
	int ret = 0;
	struct new_thread_args *p = (struct new_thread_args *)arg;

	/* Make sure we don't start until our parent has entered
	 * our thread info in the thread table. */
	//lock();
	//unlock();

	/* This has to be done here, because pthread_setspecific cannot
	 * be called by our parent on our behalf.  That's why the ti
	 * pointer is passed in the new_thread_args structure. */
	/* Synchronization is not a problem, because the only thread
	 * that relies on this call having been made is this one --
	 * no other thread can access our TSD anyway. */
#ifndef WIN32
	ret = pthread_setspecific(tsd_key1, p->ti);
	if (ret != 0) 
	{
	//	error(ret, "thread-pthread: pthread_setspecific failed: %s", strerror(errno));
	}
#else
	//
#endif
	(p->func)(p->arg);

	//lock();
//	debug("lib.thread", 0, "Thread %ld (%s) terminates.",
//		p->ti->number, p->ti->name);
	/* Must free p before signaling our exit, otherwise there is
	 * a race with ss_check_leaks at shutdown. */
//	_free(p);
//	delete_threadinfo();
//	unlock();

	return NULL;
}

long JUJU::thread_create_real(thread_func_t *func,  void *arg, const char *name="") 
{

	long ret;
#ifndef WIN32
	pthread_t id;


#else
	long	id = -1;
#endif

	struct new_thread_args *p;		
	/* We want to pass both these arguments to our wrapper function
	 * new_thread, but the pthread_create interface will only let
	 * us pass one pointer.  So we wrap them in a little struct. */
	p = (struct new_thread_args *)malloc(sizeof(*p));
	p->func = func;
	p->arg = arg;
	p->ti = (struct threadinfo *)malloc(sizeof(*(p->ti)));


	
#ifndef WIN32
	ret = pthread_create(&id, NULL, &new_thread, p);
	if (ret != 0) {	
		//error(ret, "Could not create new thread.");
		//_free(p);
		return -1;
	}
#else
	ret = _beginthreadex(NULL, 0, (unsigned int (__stdcall *)(void *))func, arg, 0, (unsigned *)&id);
//	ret = _beginthreadex(NULL, 0, new_thread, p, 0, (unsigned *)&id);
	if (ret <=0)
	{
		//error(ret, "Could not create new thread.");
		free(p);
		return -1;
	}
#endif
printf("1>");
#ifndef WIN32
	ret = pthread_detach(id);
	if (ret != 0) {
		//warning(ret, "Could not detach new thread.");
	}
#endif


		printf("Started thread (%s)", name);
//	printf("Started thread %ld (%s)", number, name);

//	debug("lib.thread", 0, "Started thread %ld (%s)", 
//		number, name);

	return ++g_nThreadNum;
}

void JUJU::sleep(long nMilliSeconds){
#ifdef WIN32
	Sleep(nMilliSeconds);
#else
//usleep(nMilliSeconds);
struct timespec req;
	int ndelay = nMilliSeconds *1000;
      req.tv_sec = ndelay / 1000000;
      req.tv_nsec = (ndelay % 1000000) * 1000;
	int      ret = nanosleep (&req, NULL);
#endif

}
