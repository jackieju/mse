

#include <sys/types.h>

#ifdef WIN32
 #include <windows.h>
 #include <objbase.h> // 4 CoCreateGuid()
 #ifdef __SDK__
  #include <psapi.h>
 #endif
#else
 #include <unistd.h>
#endif

#ifdef AIX // get memory info
	#include <procinfo.h>
	#include <sys/types.h>
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>



#include "osmacros.h"
#include "osutils.h"

int OSGetErrorNo()
{
#ifdef WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}

int OSGetLastError()
{
#ifdef WIN32
	return GetLastError();
#else
	return errno;
#endif
}


#ifndef WIN32
OS_UL GetTickCount()
{
	FILE *fp=NULL;
	fp = fopen("/proc/stat", "r");						// read cpu elapse time from sysprocess file
	if(fp == NULL)
		return 0;
	OS_UL nusermode=0, nlusermode=0, nsysmode=0, nidle=0;
	char sbuffer[sizeof(char)*101];
	memset(sbuffer, 0, sizeof(char)*101);

	int nread = fread(sbuffer, sizeof(char), 100, fp);
	if(nread <= 0)
	{
		fclose(fp);
		return 0;
	}

	char scpu[10];
	memset(scpu, 0, 10);

	sscanf(sbuffer, "%s  %d %d %d %d\n", scpu, &nusermode, &nlusermode, &nsysmode, &nidle);

	fclose(fp);
	return (nusermode+nlusermode+nsysmode+nidle)*10;					// /proc/state return 1/100 second, so 10*ret
}
#else
#endif

// 对两块内存进行“按位与”操作，结果直接写入第一块内存
// 第二块内存不能比第一块内存小
void OSBitwiseAnd(OS_UC*	pBuffer1, OS_UC* pBuffer2, OS_UL dwBuffer1Size)
{
	OS_UL i;
	if (!pBuffer1 || !pBuffer2)
		return;

	for (i = 0; i < dwBuffer1Size; i ++)
		pBuffer1[i] = pBuffer1[i] & pBuffer2[i];
}

// 对两块内存进行“按位或”操作，结果直接写入第一块内存
// 第二块内存不能比第一块内存小
void OSBitwiseInclusiveOr(OS_UC*	pBuffer1, OS_UC* pBuffer2, long dwBuffer1Size)
{
	OS_UL i;
	if (!pBuffer1 || !pBuffer2)
		return;

	for (i = 0; i < dwBuffer1Size; i ++)
		pBuffer1[i] = pBuffer1[i] | pBuffer2[i];
}

// 对两块内存进行“按位异或”操作，结果直接写入第一块内存
// 第二块内存不能比第一块内存小
void OSBitwiseExclusiveOr(OS_UC*	pBuffer1, OS_UC* pBuffer2, OS_UL dwBuffer1Size)
{
	OS_UL i;
	if (!pBuffer1 || !pBuffer2)
		return;

	for (i = 0; i < dwBuffer1Size; i ++)
		pBuffer1[i] = pBuffer1[i] ^ pBuffer2[i];
}

// 对内存进行“按位取反”操作，结果直接写入原来的内存
void OSBitwiseNot(OS_UC*	pBuffer, OS_UL dwBufferSize)
{
	OS_UL i;
	if (!pBuffer)
		return;

	for (i = 0; i < dwBufferSize; i ++)
		pBuffer[i] = ~pBuffer[i];
}

// 将time_t转换成struct tm结构，时区按照标准时区计算，该结构空间由调用者分配
void OSUTCTime(time_t* pT, struct tm* pOutTm)
{
	memcpy(pOutTm, gmtime(pT), sizeof(struct tm));
}

// 将time_t转换成struct tm结构，并且加上本机的时区，该结构空间由调用者分配
void OSLocalTime(time_t* pT, struct tm* pOutTm)
{
	memcpy(pOutTm, localtime(pT), sizeof(struct tm));
}

static	char szPath[512];

// init the directory of executable file
void OSInitCurrentDirectory(void)
{
#ifdef WIN32
	char *pPath = NULL;

	// Cut the file name, reserve the path name
	if ( GetModuleFileName( NULL, szPath, 512 ) != 0 )
	{
		if ((pPath = strrchr(szPath, '\\')) == NULL)
		{
			printf("Error in parsing path: %s, and we use default path:'.'\n", szPath);
			strcpy(szPath, ".");
		}

		*pPath = 0;
	}
	else
	{
		printf("Error in parsing path: %s, and we use default path:'.'\n", szPath);
		strcpy(szPath, ".");
	}
#else
	getcwd(szPath, 512);
#endif
}

// get the directory of executable file
char *OSGetCurrentDirectory(int iWithFileName)
{
	return szPath;
}

// get the memory info of the given process
BOOL GetProcMemInfo(unsigned long* pdwPhysicalMemByte, unsigned long* pdwVirtualMemByte)
{
	// zero return value
	*pdwPhysicalMemByte = *pdwVirtualMemByte = 0;

#if defined(WIN32)
 #ifdef __SDK__
	PROCESS_MEMORY_COUNTERS procMemCounters;
	HANDLE hThis;
	hThis = GetCurrentProcess();

	if (GetProcessMemoryInfo(hThis, &procMemCounters, sizeof(procMemCounters)))
	{
		*pdwPhysicalMemByte = procMemCounters.WorkingSetSize;
		*pdwVirtualMemByte = procMemCounters.PagefileUsage;
	}
 #endif
#elif defined(LINUX)
	char sProcFileName[MAX_PATH], sTmp[MAX_PATH], cTmp;
	long nTemp, ret = -1;
	long vsize, rss; 
	FILE *fp = NULL;
	long pid;

	pid = getpid();
	snprintf(sProcFileName, MAX_PATH, "/proc/%d/stat", pid);

	fp = fopen(sProcFileName, "r");
	if (!fp)
	{
		printf("Error in accessing %s\n", sProcFileName);
		return false;
	}

	// virtual memory size, resident set size
	// total 24 fields
	ret = fscanf(fp, "%d %s %c %d %d %d %d %d %u %u %u %u %u %d %d %d %d %d %d %u %u %d %u %u", 
		&nTemp, sTmp, &cTmp, &nTmp, &nTmp, &nTmp, &nTmp, &nTmp, &nTmp, &nTmp, &nTmp, &nTmp, 
		&nTmp/*last u*/, &nTmp, &nTmp, &nTmp, &nTmp, &nTmp, &nTmp/*last d*/, &nTmp, &nTmp, 
		&nTmp/*u u d*/, &vsize, &rss);
	if (ret != 24)
	{
		fclose(fp);
		return false;
	}

	// set return value
	*pdwPhysicalMemByte = rss * 4096;
	*pdwVirtualMemByte  = vsize;

	fclose(fp);
#else	// non-windows , non-linux
	// all os supporting 'ps -u [pid]' command
	char buf[1024]= {0};
	FILE *fp = NULL;
	char user[1024], tty[1024], sStat[1024], sStart[1024], sTime[1024], sCommand[1024];
	int pid, vmem, rss;
	double cpu, mem;
	int ret = 0;

	snprintf(buf, 1023, "ps u %d", getpid());
	buf[1024] = 0;

	fp = popen(buf, "r");
	if (!fp)
	{
		return false;
	}
	
	fgets(buf, 1024, fp);
	RETVIF(buf, false);

	fgets(buf, 1024, fp);
	RETVIF(buf, false);

	if (fp)
		fclose(fp);

//USER       PID %CPU %MEM   VSZ  RSS TTY      STAT START   TIME COMMAND
//root     10395  0.0  1.0  2508 1348 pts/2    S    18:09   0:00 -bash
	ret = sscanf(buf, "%s %d %lf %lf %d %d %s %s %s %s %s", user, &pid, &cpu, &mem,
		&vmem, &rss, &tty, &sStat, sStart, sTime, sCommand);
	
	if (ret != 11) // total 11 columns
	{
		return false;
	}
	
	*pdwPhysicalMemByte = rss;
	*pdwVirtualMemByte  = vmem;	

	return true;
#endif
	return true;
}

	/**
	<pre>
// @function:	Generate GUID
// @param:		pointer to the GUID on return
// @ret value:	0: success; -1: failed
// @origin:		yangyan
// @date:		2002-08-13
	</pre>
	*/
long OSUUIDGen( OS_UUID* pguid )
{
#ifdef WIN32
	HRESULT hr = CoCreateGuid((GUID*)pguid);
	if (S_OK == hr)
		return 0;
	else
		return -1;
#endif
}

///* 
//* return value: 
//*     1 big-endian 
//*     2 little-endian 
//*     3 unknow 
//*     4 sizeof( short ) != 2 
//*/ 
//static int OSGetByteOrder ( void ) 
//{ 
//    union 
//    { 
//        short s; 
//        char  c[ sizeof( short ) ]; 
//    } un; 
//
//    un.s = 0x0201; 
//    if ( 2 == sizeof( short ) ) 
//    { 
//        if ( ( 2 == un.c[0] ) && ( 1 == un.c[1] ) ) 
//        { 
//            puts( "big-endian" ); 
//            return( 1 ); 
//        } 
//        else if ( ( 1 == un.c[0] ) && ( 2 == un.c[1] ) ) 
//        { 
//            puts( "little-endian" ); 
//            return( 2 ); 
//        } 
//        else 
//        { 
//            puts( "unknow" ); 
//            return( 3 ); 
//        } 
//    } 
//    else 
//    { 
//        puts( "sizeof( short ) = %d", sizeof( short ) ); 
//        return( 4 ); 
//    } 
//    return( 3 ); 
//}  /* end of byte_order */ 
