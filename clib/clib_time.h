#ifndef __OS_TIME_H__
#define __OS_TIME_H__


#include "macros.h"

#ifndef WIN32
//
// System time is represented with the following structure:
//
typedef struct _SYSTEMTIME {
    OS_US wYear;
    OS_US wMonth;
    OS_US wDayOfWeek;
    OS_US wDay;
    OS_US wHour;
    OS_US wMinute;
    OS_US wSecond;
    OS_US wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

OSLIB_DLL_FUNC_HEAD VOID GetLocalTime(
  LPSYSTEMTIME lpSystemTime   // system time
);
#else
#endif

#endif


