
#ifndef __OSMACROS_H__
#define __OSMACROS_H__

#include <stdio.h>
#include <string.h>
#ifdef _MACOS
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif
#include <ctype.h>
#include <wchar.h>
#include <errno.h>

typedef unsigned long OS_UL;
typedef unsigned short OS_US;
typedef unsigned char OS_UC;

#if defined( WIN32 ) && defined (OSLIB_DLL_EXPORTS)
 #define OSLIB_DLL_FUNC_HEAD __declspec(dllexport) 
#else
 #define OSLIB_DLL_FUNC_HEAD
#endif

// in order to make this class much common, we redefine these macros here. :(
#ifndef TESTP
 #define TESTP
 #define TESTPC(p) printf("Create pointer:%p\n", p)
 #define TESTPD(p) printf("Destroy pointer:%p\n", p)
#endif

/**
 * Delete a NEWed pointer and zero it into NULL.
 */
#ifndef SAFEDELETE
#define SAFEDELETE(x) \
	do { \
		if ((x) != NULL) \
		{ \
			delete (x); \
		} \
		(x) = NULL; \
		}while(0)
#endif

#ifndef SAFEDELARY
#define SAFEDELARY(x)	\
	do { \
		if ((x) != NULL) \
		{ \
			delete [] (x); \
		} \
		(x) = NULL; \
	}while(0)
#endif

/**
 * Free a MALLOCed pointer and zero it into NULL.
 */
#ifndef SAFEFREE
#define SAFEFREE(x) \
	do { \
		if ((x) != NULL) \
		{ \
			free (x); \
		} \
		(x) = NULL; \
		}while(0)
#endif

#if !defined( UCHAR_MAX )
 #define UCHAR_MAX 255
#endif

/**
 * Return a NULL pointer if x equals to NULL.
 */
#define RETIF(x) \
	do { \
		if ((x) == NULL) \
			return; \
	} while(0)

/**
 * Return y if x equals to NULL.
 */
#define RETVIF(x, y) \
	do { \
		if ((x) == NULL) \
            return (y); \
	} while(0)

#ifdef linux
 #define FAR 
 #define dllexport
 #define dllimport 
 #define __declspec(x) x
#endif

#ifndef WIN32
 #ifdef linux
  #define __cdecl __attribute__((__cdecl__))
  #define __stdcall __attribute__((__stdcall__))
 #elif defined(_AIX)
  #define __cdecl 
  #define __stdcall 
  #define bezro(x, y) memset(x, 0, y)
 #else
  #define __cdecl 
  #define __stdcall 
 #endif

 #ifndef SQL_H_SQLCLI // for DWORD defined by DB2 client header file
  typedef unsigned long DWORD;
 #endif
 typedef unsigned short WORD;
 typedef unsigned char BYTE;
 typedef int BOOL;
 typedef long LONG;
 typedef unsigned int UINT;
 typedef unsigned int SOCKET;
 typedef struct hostent *PHOSTENT;
 typedef struct hostent HOSTENT;
 typedef struct sockaddr_in SOCKADDR_IN;
 typedef struct sockaddr_in *PSOCKADDR_IN;
 typedef struct sockaddr SOCKADDR;
 typedef struct sockaddr *PSOCKADDR;
 typedef char TCHAR, * LPTSTR;
 typedef const char *LPCSTR;
 #define TEXT(x)	x
 #ifndef NO_ERROR
  #define NO_ERROR 0L  // dderror
 #endif
 typedef void VOID;
// #define FAR far

 #define strnicmp(x, y, z) strncasecmp(x, y, z)
 #define stricmp(x, y) strcasecmp(x, y)
//
// Generic test for success on any status value (non-negative numbers
// indicate success).
//

 #define SUCCEEDED(Status) ((HRESULT)(Status) >= 0)

//
// and the inverse
//
 #define FAILED(Status) ((HRESULT)(Status)<0)


 #define TRUE			1
 #define FALSE		0
 #define INVALID_SOCKET			(SOCKET)(~0)
 #define SOCKET_ERROR					(-1)
 //#define TCP_NODELAY			1
 #define closesocket(p)				close( (p) )
 #define Sleep(p)					usleep( (p) * 1000)
 #define MAX_PATH				260
 #define _MAX_PATH				MAX_PATH
// #define FAR 						far

 // signal related
 #define SERVICE_CONTROL_STOP	SIGINT
 #define CTRL_BREAK_EVENT		SIGTERM
 #define CTRL_C_EVENT			SIGINT
 #define TESTQUIT(x) 
#else

/**
 * Break from a loop if key 'x' is pressed.
 */
#define TESTQUIT(x)	\
	if (GetKeyState(VK_CONTROL) & 0x8000)	\
	{	\
		if (GetKeyState(x) & 0x8000)	\
		{	\
			printf("Get to quit now!\n");	\
			break;	\
		}	\
	}

 #define strncasecmp(x, y, z) strnicmp(x, y, z)
 #define strcasecmp(x, y) stricmp(x, y)
 typedef int socklen_t;
#endif // ifndef WIN32

// #define ss_realloc(x, y)	OSReAlloc(x, y)
 #define ss_realloc(x, y)	realloc(x, y)

#ifdef WIN32
// for possible bugs of realloc function under WIN32 platform
static void *OSReAlloc(void *memblock, size_t new_size)
{
	void *p = NULL;
	size_t last_size = 0;

	if (!memblock)
		last_size = 0;
	else
		last_size = _msize( memblock );
	
	p = malloc(new_size);
	if (!p)
	{
		printf("Error in applying new memory space with error:%s\n", strerror(errno));
		return NULL;
	}

	// copy data
	memcpy(p, memblock, last_size);

	// release unused space
	SAFEFREE(memblock);

	return p;
};

#define snprintf	_snprintf
#define vsnprintf   _vsnprintf

#endif	// end for ifdef WIN32

// byte order related
#ifndef BIGENDIAN
 #define BIGENDIAN                           0x0000
#endif

#ifndef LITTLEENDIAN
 #define LITTLEENDIAN                        0x0001
#endif

#ifdef _AIX
 #define OS_BYTE_ORDER BIGENDIAN
#elif defined(WIN32)
 #define OS_BYTE_ORDER LITTLEENDIAN
#else // based on intel CPUs
 #define OS_BYTE_ORDER LITTLEENDIAN
#endif
// end of byte order related

#ifdef WIN32
typedef	__int16 INT16;
//typedef	__int32 INT32;
typedef	__int64 INT64;

#else
typedef char* LPSTR;
typedef short INT16;
typedef int INT32;
//typedef	int64 INT64;
typedef void* HMODULE;
typedef int FARPROC;
typedef void* PVOID;

#endif


#endif // end _H_
