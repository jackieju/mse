#ifndef __MACRO_H__
#define __MACRO_H__

#ifndef _MAX_PATH
#define _MAX_PATH 256
#endif
//#define BOOL int
#ifdef WIN32 // FOR win32
#define  snprintf _snprintf
#define FALSE 0
#define TRUE 1
#define PATH_SEPARATOR_S "\\"
#define PATH_SEPARATOR_C '\\'
#else // for linux
//#define NULL 0
#define MAX_PATH 256
#define TRUE 1
#define FALSE 2
#define PATH_SEPARATOR_S "/"
#define PATH_SEPARATOR_C '/'
#endif

#endif //__MACRO_H__

