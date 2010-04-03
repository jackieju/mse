//
//	Utility.cpp
//

#ifdef _WIN32
#include "windows.h"
//#include "afx.h"
#else
#include <sys/stat.h>
#include <sys/types.h> 
#endif
//#include "windows.h"
#include <string>
//#include <sslib.h>
//#include "assert.h"
#define assert(s)	
#include "stdio.h"
#include <errno.h>
#include "macros.h"
#include "utility.h"
#include <time.h>



 using namespace JUJU;



/*CSingleWriteMultiRead::CSingleWriteMultiRead(long lMaxReaders)
{
	m_hMutexNoWriter = CreateMutex(NULL, FALSE, NULL);

	m_hEventNoReaders = CreateEvent(NULL, TRUE, TRUE, NULL);

	m_hSemNumReaders = CreateSemaphore(NULL, 0, lMaxReaders, NULL);

	if ((NULL == m_hMutexNoWriter)	|| 
		(NULL == m_hEventNoReaders)	||
		(NULL == m_hSemNumReaders) ){
		//TRACE("No enough system resource for create SRMW Object\r\n");
	}

	assert(m_hMutexNoWriter && m_hEventNoReaders && m_hSemNumReaders);
}


CSingleWriteMultiRead::~CSingleWriteMultiRead()
{
	if (m_hMutexNoWriter)
		CloseHandle(m_hMutexNoWriter);
	if (m_hEventNoReaders) 
		CloseHandle(m_hEventNoReaders);
	if (m_hSemNumReaders)
		CloseHandle(m_hSemNumReaders);
}



DWORD CSingleWriteMultiRead::WaitToWrite(DWORD dwTimeOut)
{
	DWORD dw;
	HANDLE aHandles[2];

	aHandles[0] = m_hMutexNoWriter;
	aHandles[1] = m_hEventNoReaders;

	dw = WaitForMultipleObjects(2, aHandles, TRUE, dwTimeOut);

	if (dw == WAIT_TIMEOUT){
	//	TRACE ("CSingleWriteMultiRead WaitToWrite Time OUT!");
	}

	return (dw);
}

void CSingleWriteMultiRead::DoneWriting()
{
	ReleaseMutex(m_hMutexNoWriter);
}


DWORD CSingleWriteMultiRead::WaitToRead(DWORD dwTimeOut)
{
	DWORD dw;
	LONG lPreviousCount;

	dw = WaitForSingleObject(m_hMutexNoWriter, dwTimeOut);

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

	return (dw);
}

void CSingleWriteMultiRead::DoneReading()
{
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
}*/

//////////////////////////////////////////////////////////////////////////////////////////
// quick sort function
//////////////////////////////////////////////////////////////////////////////////////////
#define SWAPINIT(a, es) swaptype =         \
    (a-(char*)0 | es) % sizeof(long) ? 2 : es > sizeof(long);

#define swapcode(TYPE, parmi, parmj, n) {  \
    register TYPE *pi = (TYPE *) (parmi);  \
    register TYPE *pj = (TYPE *) (parmj);  \
    do {                                   \
        register TYPE t = *pi;             \
        *pi++ = *pj;                       \
        *pj++ = t;                         \
    } while ((n -= sizeof(TYPE)) > 0);     \
}

#include <stddef.h>

static void swapfunc(char *a, char *b, size_t n, int swaptype)
{   if (swaptype <= 1) swapcode(long, a, b, n)
    else swapcode(char, a, b, n)
}

#define swap(a, b)                         \
    if (swaptype == 0) {                   \
        t = *(long*)(a);                   \
        *(long*)(a) = *(long*)(b);         \
        *(long*)(b) = t;                   \
    } else                                 \
        swapfunc(a, b, es, swaptype)

#define PVINIT(pv, pm)                                \
    if (swaptype != 0) { pv = a; swap(pv, pm); }      \
    else { pv = (char*)&v; *(long*)pv = *(long*)pm; }

#define vecswap(a, b, n) if (n > 0) swapfunc(a, b, n, swaptype)


static char *med3_1(char *a, char *b, char *c, long (*cmp)(char *, char*, long , long, long), long p1, long p2, long p3)
{	return cmp(a, b, p1, p2, p3) < 0 ?
		  (cmp(b, c, p1, p2, p3) < 0 ? b : cmp(a, c, p1, p2, p3) < 0 ? c : a)
		: (cmp(b, c, p1, p2, p3) > 0 ? b : cmp(a, c, p1, p2, p3) > 0 ? c : a);
}
static char *med3_2(char *a, char *b, char *c, long (*cmp)(char *, char*, long , long, long, long, long, long), long p1, long p2, long p3, long q1, long q2, long q3)
{	return cmp(a, b, p1, p2, p3, q1, q2, q3) < 0 ?
		  (cmp(b, c, p1, p2, p3, q1, q2, q3) < 0 ? b : cmp(a, c, p1, p2, p3, q1, q2, q3) < 0 ? c : a)
		: (cmp(b, c, p1, p2, p3, q1, q2, q3) > 0 ? b : cmp(a, c, p1, p2, p3, q1, q2, q3) > 0 ? c : a);
}

void JUJU::QSort2(char *a, size_t n, size_t es, long (*cmp)(char*, char*, long, long, long, long, long, long), long p1, long p2, long p3, long q1, long q2, long q3)
{
	assert(a && cmp && es);
	if (n == 0)
		return;

	char *pa, *pb, *pc, *pd, *pl, *pm, *pn, *pv;
	int r, swaptype;
	long t, v;
	size_t s;

	SWAPINIT(a, es);
	if (n < 7) {	 /* Insertion sort on smallest arrays */
		for (pm = a + es; pm < a + n*es; pm += es)
			for (pl = pm; pl > a && cmp(pl-es, pl, p1, p2, p3, q1, q2, q3) > 0; pl -= es)
				swap(pl, pl-es);
		return;
	}
	pm = a + (n/2)*es;    /* Small arrays, middle element */
	if (n > 7) {
		pl = a;
		pn = a + (n-1)*es;
		if (n > 40) {    /* Big arrays, pseudomedian of 9 */
			s = (n/8)*es;
			pl = med3_2(pl, pl+s, pl+2*s, cmp, p1, p2, p3, q1, q2, q3);
			pm = med3_2(pm-s, pm, pm+s, cmp, p1, p2, p3, q1, q2, q3);
			pn = med3_2(pn-2*s, pn-s, pn, cmp, p1, p2, p3, q1, q2, q3);
		}
		pm = med3_2(pl, pm, pn, cmp, p1, p2, p3, q1, q2, q3); /* Mid-size, med of 3 */
	}
	PVINIT(pv, pm);       /* pv points to partition value */
	pa = pb = a;
	pc = pd = a + (n-1)*es;
	for (;;) {
		while (pb <= pc && (r = cmp(pb, pv, p1, p2, p3, q1, q2, q3)) <= 0) {
			if (r == 0) { swap(pa, pb); pa += es; }
			pb += es;
		}
		while (pb <= pc && (r = cmp(pc, pv, p1, p2, p3, q1, q2, q3)) >= 0) {
			if (r == 0) { swap(pc, pd); pd -= es; }
			pc -= es;
		}
		if (pb > pc) break;
		swap(pb, pc);
		pb += es;
		pc -= es;
	}
	pn = a + n*es;
	s = min(pa-a,  pb-pa   ); vecswap(a,  pb-s, s);
	s = min(pd-pc, pn-pd-es); vecswap(pb, pn-s, s);
    if ((s = pb-pa) > es) QSort2(a,    s/es, es, cmp, p1, p2 ,p3, q1, q2, q3);
	if ((s = pd-pc) > es) QSort2(pn-s, s/es, es, cmp, p1, p2, p3, q1, q2, q3);
}

void JUJU::QSort1(char *a, size_t n, size_t es, long (*cmp)(char*, char*, long, long, long), long p1, long p2, long p3)
{
	assert(a && cmp && es);
	if (n == 0)
		return;

	char *pa, *pb, *pc, *pd, *pl, *pm, *pn, *pv;
	int r, swaptype;
	long t, v;
	size_t s;

	SWAPINIT(a, es);
	if (n < 7) {	 /* Insertion sort on smallest arrays */
		for (pm = a + es; pm < a + n*es; pm += es)
			for (pl = pm; pl > a && cmp(pl-es, pl, p1, p2, p3) > 0; pl -= es)
				swap(pl, pl-es);
		return;
	}
	pm = a + (n/2)*es;    /* Small arrays, middle element */
	if (n > 7) {
		pl = a;
		pn = a + (n-1)*es;
		if (n > 40) {    /* Big arrays, pseudomedian of 9 */
			s = (n/8)*es;
			pl = med3_1(pl, pl+s, pl+2*s, cmp, p1, p2, p3);
			pm = med3_1(pm-s, pm, pm+s, cmp, p1, p2, p3);
			pn = med3_1(pn-2*s, pn-s, pn, cmp, p1, p2, p3);
		}
		pm = med3_1(pl, pm, pn, cmp, p1, p2, p3); /* Mid-size, med of 3 */
	}
	PVINIT(pv, pm);       /* pv points to partition value */
	pa = pb = a;
	pc = pd = a + (n-1)*es;
	for (;;) {
		while (pb <= pc && (r = cmp(pb, pv, p1, p2, p3)) <= 0) {
			if (r == 0) { swap(pa, pb); pa += es; }
			pb += es;
		}
		while (pb <= pc && (r = cmp(pc, pv, p1, p2, p3)) >= 0) {
			if (r == 0) { swap(pc, pd); pd -= es; }
			pc -= es;
		}
		if (pb > pc) break;
		swap(pb, pc);
		pb += es;
		pc -= es;
	}
	pn = a + n*es;
	s = min(pa-a,  pb-pa   ); vecswap(a,  pb-s, s);
	s = min(pd-pc, pn-pd-es); vecswap(pb, pn-s, s);
	if ((s = pb-pa) > es) QSort1(a,    s/es, es, cmp, p1, p2 ,p3);
	if ((s = pd-pc) > es) QSort1(pn-s, s/es, es, cmp, p1, p2, p3);
}
/*//////////////////////////////////////////////////////////////////////////////////////////
// sprintf function
//////////////////////////////////////////////////////////////////////////////////////////
long Sprintf (
        char *string,
        size_t count,
        const char *format,
        ...
        )

{
        FILE str;
        FILE *outfile = &str;
        va_list arglist;
        long retval;

        va_start(arglist, format);

        assert(string != NULL);
        assert(format != NULL);

        outfile->_flag = _IOWRT|_IOSTRG;
        outfile->_ptr = outfile->_base = string;

        outfile->_cnt = count;

  retval = _output(outfile,format,arglist);

        _putc_lk('\0',outfile); 

        return(retval);
}
*/

/**
函数声明：	BOOL CheckDir(const char* strDir, bool bDir)
函数功能：	检查路径是否存在， 如果不存在就生成路径, 如果是一个单独的文件名， 则忽略
参数说明：	
			[IN]const char* strDir	-	路径名(如果是目录名则最后一定要加'/')
返 回 值：	BOOL  - 成功或失败
编 写 人：	居卫华
完成日期：	2002-3-3
**/
bool JUJU::CheckPath(const char* strDir)
{
	assert(strDir);

	char strCopy[MAX_PATH];
//	char strTmp[MAX_PATH];
	char* p;
	
	strcpy(strCopy, strDir);

	if (strDir[strlen(strCopy) - 1] == PATH_SEPARATOR)	// if it is a dir check it
	{ 
		if (chdir(strDir) == 0)
			return TRUE;

		// if it not exist, get its parent dir
		while (strCopy[strlen(strCopy) - 1] == PATH_SEPARATOR)
			strCopy[strlen(strCopy) - 1] = '\0';

		p = strrchr(strCopy, PATH_SEPARATOR);
        if (p != NULL) 
	    	*(p + sizeof(char)) ='\0';
        else{
            // if parent dir not exist, create it
#ifdef WIN32 
            if (mkdir(strDir/*,0777*/) != 0)
#else
            if (mkdir(strDir,0777) != 0)
#endif
                    //	if (mkdir(strCopy/*,0777*/) != 0)
                    if (errno != EEXIST)
                        return FALSE;
                    return TRUE;
        }

	}
	else									// if it includes a file name, truncate it
	{
        p = strrchr(strCopy, PATH_SEPARATOR);
        if (p == NULL)
            return TRUE;
       	*(p + sizeof(char)) ='\0';
    }
    
    
    // check parent dir
    //if (CheckPath((const char*)strCopy))
    //    return FALSE;
    CheckPath((const char*)strCopy);
    // if parent dir not exist, create it
#ifdef WIN32 
    if (mkdir(strDir/*,0777*/) != 0)
#else
     if (mkdir(strDir,0777) != 0)
#endif
            //	if (mkdir(strCopy/*,0777*/) != 0)
     if (errno != EEXIST)
        return FALSE;
     return TRUE;
}

#ifdef WIN32
//bool IsFileExist(char* fileName, WIN32_FIND_DATA &FileInfo)
//{
//	assert(fileName);
//	assert(strlen(fileName));
//
//	memset(&FileInfo, 0, sizeof(WIN32_FIND_DATA));
//
//	WIN32_FIND_DATA fileInfo;
//        HANDLE fileHandle = FindFirstFile(fileName,&fileInfo);
//	if(fileHandle!=INVALID_HANDLE_VALUE)
//		if(fileInfo.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
//		{
//			if(FindNextFile(fileHandle,&fileInfo))
//			{
//				memcpy(&FileInfo, &fileInfo, sizeof(WIN32_FIND_DATA));
//				FindClose(fileHandle);
//				return true;
//			}
//		}
//		else
//		{
//			memcpy(&FileInfo, &fileInfo, sizeof(WIN32_FIND_DATA));
//			FindClose(fileHandle);
//			return true;
//		}
//	FindClose(fileHandle);
//	return false;
//}
#endif

/**
函数声明：	void Log(char* filename, char* msg, char* sourcefile, long line, char* format)
函数功能：	输出格式字符串到文件, 注意文件的路径中不要只写一个'\', 格式中需自行添加回车.
参数说明：
			[IN]char* filename		-	文件名
			[IN]char* msg			-	所记录的内容正文
			[IN]char* sourcefile	-	发生log的源文件名
			[IN]long line			-	log发生的行数
			[IN]char* format		-	log格式(可以是"%s; file=%s; line=%05d")
返 回 值：	void  - 空
编 写 人：	居卫华
完成日期：	2002-1-4
**/

void JUJU::SystemLog(char* filename, char* msg, char* sourcefile, long line, char* format)
{
	if ( !msg || !sourcefile|| line < 0 || !format)
	{
		//Beep(1000, 500);
		//beep();
		return;
	}
	time_t t;
        struct tm *ptm;
        time(&t);

	// generate file name
	char fname[MAX_PATH];
	if (!filename || strlen(filename) == 0)
	{
		sprintf(fname, "%04d%02d%02d",ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
	}
	else
	{
		char temp[MAX_PATH];
		strcpy(temp, filename);
		// get extension name
		char *p = strrchr(temp, '.');
		if (p)
		{
			*p = '\0';
			sprintf(fname, "%s%04d%02d%02d.%s", temp, ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, p+1);
		}
		else
			sprintf(fname, "%s%04d%02d%02d", temp,ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday); 
	}
	FILE* file = NULL;
	file = fopen(fname, "a+");
	if (!file)
	{
		//Beep(1000, 500);
		//beep();
		return;
	}
//	fprintf(file, "%04d-%02d-%02d %02d:%02d:%02d\t", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	fprintf(file, format, msg, sourcefile, line);
	fclose(file);
	return;
}
#if 0
char* CreateFormatString(long init_size, char* format, ...)
{
	long size = 1000;
	if (init_size > 0 )
		size = init_size;

	long count = 0;

	char* p = new char[size];
	if (p == NULL)
		return NULL;

	memset(p, 0, size);
	va_list arglist;
	long loop_num = 0;	

	while (1)
	{
		va_start(arglist, format);
		count = _vsnprintf(p, size - 1, format, arglist);
		va_end(arglist);

		if (count < 0)
		{
			size += size;
			char *pNew = new char[size];
			memset(pNew, 0, size); 
			delete p;
			p = pNew;
			loop_num ++;
			if (loop_num >= 10)
			{
				loop_num = 0;
				Sleep(10);
			}
			continue;
		}
		break;
	}
	return p;
}

#endif
void* JUJU::DllNew(size_t size)
{
	if (size < 0)
		return (void*)new char[size];
	return NULL;
}


#ifdef _WIN32
template <class LIST, class S, class E>
long JUJU::binary_search(LIST array, long start, S s, long number, int (__cdecl* compare)(S, E), bool& bFound)
{
	
	bFound = false;
	
	if (number < 0)
		//	throw new CExp(CExp::err_invalid_param, "binary_search() number < 0", __FILE__, __LINE__);
		assert(0);
	
	if (number == 0)
		return 0;
	
	//	*index = -1;
	int low, high, mid, ret;
	
	low = start;
	high = start + number-1;
	mid = -1;
	
	while (high >= low )//&&compare(array[low], array[high]) <= 0 )
	{
		mid = (low+high)/2;
		ret = compare(s, array[mid]);
		if (ret == 0)
		{			
			bFound = false;
			return mid;
		}
		else
		{
			if (ret < 0) 
				high = mid - 1;
			if (ret > 0) //table[i] > table[k]
				low = mid + 1;
		}	
	}	
	if ( ret > 0)
		return mid +1;
	if ( ret < 0)
		return mid;
}
unsigned char* JUJU::ReadFile(char* szFileName, long &len)
{
	len = 0;
	FILE* file = NULL;
	file = fopen(szFileName, "rb");
	if (!file)
	{
		return NULL;
	}
	
	// 移到文件尾
	if (fseek(file, 0, SEEK_END) != 0)
	{
		fclose(file);
		return NULL;
	}
	
	// 取得文件长度
	long size = 0;
	size = ftell(file);
	if (size <= 0)
	{
		fclose(file);
		return NULL;
	}
	
	// 移回文件头，准备读文件
	if (fseek(file, 0, SEEK_SET) != 0)
	{
		fclose(file);
		return NULL;
	}
	
	// read data
	unsigned char* buffer;
	buffer = new BYTE[size];
	if (buffer == NULL)
	{
		fclose(file);
		return NULL;
	}
	int readret;
	readret = fread(buffer, sizeof(char), size, file);
	fclose(file);
	if (readret < size)
	{
		SAFEDELETE(buffer);
		return NULL;
	}

	len = size;
	return buffer;
}

void JUJU::WriteFile(char* filename, unsigned char* buffer, long &len, char* flag)
{
	FILE* f = fopen(filename, flag);
	if (f == NULL)
		return;

	len = fwrite(buffer, 1, len, f);

	fclose(f);

}

#endif
void JUJU::DllDelete(unsigned char* p )
{
	if (p)
		delete p;
}

void JUJU::DeleteStr(char* p )
{
	if (p)
		delete p;
}

wchar_t* JUJU::atou(char* pstr){
	wchar_t* jstr_u = NULL;
#ifdef WIN32
	int len = MultiByteToWideChar(CP_ACP, 0, pstr, -1, NULL, NULL);
	jstr_u = new wchar_t[len + 1];
	if ( jstr_u ) // Not NULL
	{
		memset(jstr_u, 0, (len + 1)*sizeof(wchar_t));
		// Transfer
		MultiByteToWideChar(CP_ACP, 0, pstr, -1, jstr_u, len);
	}

	return jstr_u;

#else	// linux

	size_t in_len = strlen(pstr), out_len = in_len, ret = 0;
	int i;

	
	wchar_t wstr_u[1024] = {0};
	
	char *pTmp = setlocale(LC_CTYPE, "zh_CN");
	if (!pTmp)
	{
		fprintf(stdout, "can't set locale:zh_CN\n");
		//return NULL;
	}

	if ((ret = mbstowcs(wstr_u, pstr, 1024/*, &state*/)) == (size_t) -1)
	{
		fprintf(stdout, "couldn't convert.\n");
		return NULL;
	}

	return wstr_u;

#endif
	


}


void JUJU::atou(char* pstr, wchar_t* jstr_u, int max_len){


#ifdef WIN32
	
	
	if ( jstr_u ) // Not NULL
	{
		memset(jstr_u, 0, max_len*sizeof(wchar_t));
		// Transfer
		MultiByteToWideChar(CP_ACP, 0, pstr, -1, jstr_u, max_len);
	}

	return;

#else	// linux

	size_t in_len = strlen(pstr), out_len = in_len, ret = 0;
	int i;

	char *pTmp = setlocale(LC_CTYPE, "zh_CN");
	if (!pTmp)
	{
		fprintf(stdout, "can't set locale:zh_CN\n");
		//return NULL;
	}

	if ((ret = mbstowcs(jstr_u, pstr, max_len/*, &state*/)) == (size_t) -1)
	{
		fprintf(stdout, "couldn't convert.\n");
		return ;
	}

	return;

#endif
	


}
char* JUJU::utoa(wchar_t* uc){
	
#ifdef WIN32
	long length = wcslen(uc);
	int out_len = WideCharToMultiByte(CP_ACP, 0, uc, length, NULL, 0, NULL, NULL);
	char * str = new char[out_len + 1];
	if ( str ) // Not NULL
	{
		memset(str, 0, out_len + 1);
		// Transfer
		WideCharToMultiByte(CP_ACP, 0, uc, length, str, out_len, NULL, NULL);
	}
#else	// linux
	long length=wcslen(uc);

	mbstate_t state;
	size_t ret = 0;
	int i;
	size_t out_len = length * 6;
	wchar_t wpTmp[1024] = {0};
	char *pTmp = NULL;

	if (!wpTmp)
	{
		fprintf(stderr, "out of memory\n");
		return NULL;
	}

	for (i=0; i<length  && i < 1022; i++)
	{
		wpTmp[i] = (wchar_t)uc[i];
	}
	wpTmp[i] = L'\0';

	// provide enough length for converting
	char* str = new char[out_len + 1];
	if ( str ) // Not NULL
	{
		memset(str, 0, out_len + 1);

		char *pTmp = setlocale(LC_CTYPE, "zh_CN");
		if (!pTmp)
		{
			fprintf(stderr, "can't set locale:zh_CN\n");
			//return NULL;
		}
		if ((ret = wcstombs(str, wpTmp, out_len)) == (size_t)-1)
		{
			fprintf(stderr, "content:%s, locale:%s, error:%s\n", str, pTmp, strerror(errno));
			fprintf(stderr, "language:%s\n", nl_langinfo(CODESET));
			fprintf(stderr, "result:%d\n", ret);
			SAFEDELETE(str);
			return NULL;
		}
	}
#endif
	return str;
	}

void JUJU::utoa(wchar_t* uc, char* str, int max_len){
  long length = wcslen(uc);
#ifdef WIN32
	if ( str ) // Not NULL
	{
		memset(str, 0, max_len);
		// Transfer
		WideCharToMultiByte(CP_ACP, 0, uc, length, str, max_len-1, NULL, NULL);
	}
	return;
#else	// linux
	mbstate_t state;
	size_t ret = 0;
	int i;
	wchar_t wpTmp[1024] = {0};
	char *pTmp = NULL;

	if (!wpTmp)
	{
		fprintf(stderr, "out of memory\n");
		return;
	}

	for (i=0; i<length  && i < 1022; i++)
	{
		wpTmp[i] = (wchar_t)uc[i];
	}
	wpTmp[i] = L'\0';

	// provide enough length for converting

	if ( str ) // Not NULL
	{
		memset(str, 0, max_len);

		char *pTmp = setlocale(LC_CTYPE, "zh_CN");
		if (!pTmp)
		{
			fprintf(stderr, "can't set locale:zh_CN\n");
			//return NULL;
		}
		if ((ret = wcstombs(str, wpTmp, max_len-1)) == (size_t)-1)
		{
			fprintf(stderr, "content:%s, locale:%s, error:%s\n", str, pTmp, strerror(errno));
			fprintf(stderr, "language:%s\n", nl_langinfo(CODESET));
			fprintf(stderr, "result:%d\n", ret);
			SAFEDELETE(str);
			return;
		}
	}
#endif
	return;
	}
// get file name excluding extension
std::string JUJU::getFileName(char* fullName){

	//char* p = strrchr(fullName, '.');
	std::string f = fullName;
	int pos = f.rfind(".");
	std::string fn = f.substr(0	, pos);
	return fn;
}

// get file extension name
std::string JUJU::getFileExt(char* fullName){
	std::string f = fullName;
	int pos2 = f.rfind(".");
	int pos1 = f.rfind(PATH_SEPARATOR);
	if (pos1 < 0) pos1 = 0;
	std::string fn = f.substr(pos1, pos2);
	return fn;
};

// get file extension name
std::string JUJU::getFilePath(char* fullName){
	std::string f = fullName;
	int pos1 = f.rfind(PATH_SEPARATOR);
	if (pos1 < 0) {
		f = "";
		return f;
	}
	std::string fn = f.substr(0, pos1+1);
	return fn;
};
