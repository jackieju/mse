#ifndef __OS_H__
#define __OS_H__

#define NDEBUG 1

#include <stdio.h>
#include <ctype.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#ifdef WIN32
	#include <winsock2.h>
	#include <ws2tcpip.h>
#else
	#include <dirent.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <sys/time.h>
	#include <sys/timeb.h>
	#include <netdb.h>
	#include <fcntl.h>
#endif

#ifdef WIN32
	#define __func__ "Unkown function"
#ifndef __FUNCTION__
	#define __FUNCTION__ __func__
#endif
#endif

#include "osmacros.h"
#include "CSS_LOCKEX.h"
#include "LogCache.h"
#include "thread.h"
#include "ConfigFile.h"
#include "socket.h"
#include "osutils.h"
#include "ostime.h"
#include "str.h"

#if 0
#include "sm.h"
#include "sstime.h"
#include "ssstr.h"
#include "base64.h"
#include "ssstring.h"
#include "ssmd5.h"
#include "ssdes3.h"
#include "ssfile.h"
#include "ssLogLevel.h"
#include "LogBase.h"
#include "list.h"
#include "ha.h"
#include "link_list.h"
#include "queue.h"
#include "qm.h"
#include "sa.h"

#include "snmp/asn1.h"
#endif


/**
 * <pre>
 * sslib init.
 * Parameters: 
 *				None
 * Return value:
 *				None
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD void OSLibInit(void);
void OSLibInit(void);
/**
 * <pre>
 * sslib destruction.
 * Parameters: 
 *				None
 * Return value:
 *				None
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD void OSLibShutdown(void);

/**
 * <pre>
 * sslib version and build number.
 * Parameters: 
 *				None
 * Return value:
 *				version and build number in ansi string style.
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD char *OSLibGetVersion(void);

/**
 * main thread status variable.
 */
extern volatile sig_atomic_t main_status;



#endif //__OS_H__
