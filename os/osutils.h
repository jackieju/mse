
#ifndef __OS_UTILS_H__
#define __OS_UTILS_H__

#include <time.h>
#include "osmacros.h"

typedef struct _OS_UUID {          // size is 16
    OS_UL Data1;
    OS_US   Data2;
    OS_US   Data3;
    OS_UC  Data4[8];
} OS_UUID;

#ifndef WIN32
OS_UL GetTickCount();
#else
#endif

/**
 * <pre>
 * Get error number.
 * Parameters:
 *				None
 * Return values:
 *				error number
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSGetErrorNo();

/**
 * <pre>
 * Get error number.
 * Parameters:
 *				None
 * Return values:
 *				error number
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD int OSGetLastError();

/**
 * <pre>
 * 对两块内存进行“按位与”操作，结果直接写入第一块内存
 * 第二块内存不能比第一块内存小
 * Parameters:
 *				pBuffer1, the first memory space
 *				pBuffer2, the second memory space
 *				dwBuffer1Size, the length of the first buffer
 * Return values:
 *				None
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD void OSBitwiseAnd(OS_UC*	pBuffer1, OS_UC* pBuffer2, OS_UL dwBuffer1Size);

/**
 * <pre>
 * 对两块内存进行“按位或”操作，结果直接写入第一块内存
 * 第二块内存不能比第一块内存小
 * Parameters:
 *				pBuffer1, the first memory space
 *				pBuffer2, the second memory space
 *				dwBuffer1Size, the length of the first buffer
 * Return values:
 *				None
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD void OSBitwiseInclusiveOr(OS_UC*	pBuffer1, OS_UC* pBuffer2, OS_UL dwBuffer1Size);

/**
 * <pre>
 * 对两块内存进行“按位异或”操作，结果直接写入第一块内存
 * 第二块内存不能比第一块内存小
 * Parameters:
 *				pBuffer1, the first memory space
 *				pBuffer2, the second memory space
 *				dwBuffer1Size, the length of the first buffer
 * Return values:
 *				None
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD void OSBitwiseExclusiveOr(OS_UC*	pBuffer1, OS_UC* pBuffer2, OS_UL dwBuffer1Size);

/**
 * <pre>
 * 对内存进行“按位取反”操作，结果直接写入原来的内存
 * Parameters:
 *				pBuffer1, the first memory space
 *				pBuffer2, the second memory space
 *				dwBuffer1Size, the length of the first buffer
 * Return values:
 *				None
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD void OSBitwiseNot(OS_UC*	pBuffer1, OS_UC* pBuffer2, OS_UL dwBuffer1Size);

/**
 * <pre>
 * 将time_t转换成struct tm结构，时区按照标准时区计算，该结构空间由调用者分配
 * Parameters:
 *				pT, source time_t variable address
 *				pOutTm, output tm variable address
 * Return values:
 *				None
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD void OSUTCTime(time_t* pT, struct tm* pOutTm);

/**
 * <pre>
 * 将time_t转换成struct tm结构，并且加上本机的时区，该结构空间由调用者分配
 * Parameters:
 *				pT, source time_t variable address
 *				pOutTm, output tm variable address
 * Return values:
 *				None
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD void OSLocalTime(time_t* pT, struct tm* pOutTm);

/**
 * <pre>
 * get the directory of executable file.
 * Parameters:
 *				iWithFileName, whether to get filename or just directory name
 * Return values:
 *				result string
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD char *OSGetCurrentDirectory(int iWithFileName = 0);

/**
 * <pre>
 * init the directory of executable file.
 * Parameters:
 *				None
 * Return values:
 *				None
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD void OSInitCurrentDirectory(void);

/**
 * <pre>
 * get the memory info of the given process.
 * Parameters:
 *				pdwPhysicalMemByte, output physical memory size
 *				pdwVirtualMemByte, output virtual memory size
 * Return values:
 *				None
 * </pre>
 */
OSLIB_DLL_FUNC_HEAD BOOL GetProcMemInfo(unsigned long* pdwPhysicalMemByte, unsigned long* pdwVirtualMemByte);

/**
	<pre>
// @function:	Generate GUID
// @param:		pointer to the GUID on return
// @ret value:	0: success; -1: failed
// @origin:		yangyan
// @date:		2002-08-13
	</pre>
*/
OSLIB_DLL_FUNC_HEAD long OSUUIDGen( OS_UUID* pguid );

#endif
