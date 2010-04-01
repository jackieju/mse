#ifdef WIN32
 #include <windows.h>
#endif

#include <time.h>
#include "ostime.h"

#ifndef WIN32
VOID GetLocalTime(
  LPSYSTEMTIME lpSystemTime   // system time
)
{
	time_t tNow;
	tm tmNow;

	time(&tNow);
	tmNow = *localtime(&tNow);

	lpSystemTime->wYear = tmNow.tm_year + 1900;
	lpSystemTime->wMonth = tmNow.tm_mon + 1;
	lpSystemTime->wDay = tmNow.tm_mday;
	lpSystemTime->wHour = tmNow.tm_hour;
	lpSystemTime->wMinute = tmNow.tm_min;
	lpSystemTime->wSecond = tmNow.tm_sec;
}
#endif
