#ifndef JUJU_THREAD_H
#define JUJU_THREAD_H
#include <string>

#ifdef WIN32
#include <windows.h>
#endif



namespace JUJU{
typedef void thread_func_t(void *arg);
void sleep(long nMilliSeconds);
long thread_create_real(thread_func_t *func,  void *arg, const char *name);

}

#endif

