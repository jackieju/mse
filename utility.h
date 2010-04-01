//
//	utility.h
//

#ifndef _UTILITY_H
#define _UTILITY_H

//#include <utility.h>
#ifdef WIN32
#include <direct.h>
#endif

#ifdef WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

typedef std::vector<std::string>			string_array;

namespace utility
{

#ifndef SAFERELEASE
#define SAFERELEASE(p)	\
	{if (p) {(p)->Release(); p=NULL;}}
#endif	//SAFERELEASE

#ifndef SAFEDELETE
#define SAFEDELETE(p)	\
	{if (p) { delete p; p = NULL;}}
#endif	//SAFEDELETE
	
#ifndef SAFEDELETEA
#define SAFEDELETEA(p)	\
	{if (p) {delete []p; p = NULL;}}
#endif

#ifndef SAFEFREESTRING
#define SAFEFREESTRING(p)	\
	{if (p)	{ SysFreeString(p); p = NULL;}}
#endif	//SAFEFREESTRING

#ifndef CHECKHR
#define CHECKHR(x)	\
	{hr = x; if (FAILED(hr)) goto CleanUp;}
#endif	//CHECKHR

#ifndef CHECKNODE
#define CHECKNODE(p)	\
	{if (p==NULL) {hr=E_FAIL; goto CleanUp;}}
#endif	//CHECKNODE

#ifndef CHECKRETURN
#define CHECKRETURN(x,p)\
	{hr = x; if( FAILED(hr) || !p) goto CleanUp;}
#endif	//CHECKRETURN

#ifndef min
#define min(a,b) (((a)<(b)) ? (a) : (b))
#endif	

#ifndef InstanceOf
#define InstanceOf(o, c) (typeid(o) == typeid(c))
#endif

/*class CSingleWriteMultiRead{
public:
	CSingleWriteMultiRead(long lMaxReaders = 0x7FFFFFFF);
	~CSingleWriteMultiRead();

	DWORD WaitToWrite(DWORD dwTimeOut = INFINITE);
	void DoneWriting();
	DWORD WaitToRead(DWORD dwTimeOut = INFINITE);
	void DoneReading();

private:
	HANDLE m_hMutexNoWriter;
	HANDLE m_hEventNoReaders;

	HANDLE m_hSemNumReaders;
};*/
void QSort1(char *a, size_t n, size_t es, long (*cmp)(char*, char*, long, long, long), long p1, long p2, long p3);
void QSort2(char *a, size_t n, size_t es, long (*cmp)(char*, char*, long, long, long, long, long, long), long p1, long p2, long p3, long q1, long q2, long q3);
bool CheckPath(const char* strDir);

#ifdef WIN32
bool IsFileExist(char* fileName, WIN32_FIND_DATA &FileInfo);
#define beep() Beep(1000,500) 
#else
//#include <curses.h>
#endif

void SystemLog(char* filename, char* msg, char* sourcefile, long line, char* format = "%s, \tsource=%s, \tline=%d\n");

// create a string with speiciifed string, the return string must be deleted by caller
// the demerit of this function is:
// he will allocate buffer on heap, so maybe slow than other string function
char* __stdcall    CreateFormatString(long init_size, char* format, ...);
void* DllNew(size_t size);
template <class LIST, class S, class E>
long binary_search(LIST array, long start, S s, long number, int (__cdecl* compare)(S, E), bool& bFound);
void DllDelete(unsigned char* p );
unsigned char* ReadFile(char* szFileName, long &len);
void WriteFile(char* filename, unsigned char* buffer, long &len, char* flag = "wb");
void DeleteStr(char* p );
int CalcDayDiff(int a, int b);
int atoi_hex(char* str);

/*
class BUFFER{
public:
	BUFFER(unsigned char* p, long len);
	~BUFFER();
	unsigned char* Get(long &len);
private:
	unsigned char* p;
	long	len;
};
class CONFIG{
public:
	CONFIG();
	~CONFIG();
	union config_value{
		long l;
		double real;
		std::string string;
		BUFFER buffer;
	};
	char* GetStrItem(char* name);
	long GetLongItem(char* name);
	double GetRealItem(char* name);
	void AddItem(config_value* value){
	}
private:
	config_value* Search(char* name);
private:
	std::map<std::string, config_value>	m_map;
};*/

#ifdef _DEBUG
#define DEBUG(x)	printf("debug: ""%s, %d@%s\n", x, __LINE__, __FILE__)
#define DEBUG0(x)	printf("debug: ""%s, %d@%s\n", x, __LINE__, __FILE__)
#define DEBUG1(x, p1)	printf("debug: "x", %d@%s\n", p1, __LINE__, __FILE__)
#define DEBUG2(x, p1, p2)	printf("debug: "x", %d@%s\n", p1, p2, __LINE__, __FILE__)
#define DEBUG3(x, p1, p2, p3)	printf("debug: "x", %d@%s\n", p1, p2, p3, __LINE__, __FILE__)
#else
#define DEBUG(X)
#define DEBUG0(x)
#define DEBUG1(x, p1)
#define DEBUG2(x, p1, p2)
#define DEBUG3(x, p1, p2, p3)
#endif // _DEBUG

#ifdef _TRACE

#ifndef TRACE
#define TRACE(x)  printf("TRACE: ""%s, %d@%s\n", x, __LINE__, __FILE__)
#endif

#ifndef TRACE0
#define TRACE0(x) printf("TRACE: ""%s, %d@%s\n", x, __LINE__, __FILE__)
#endif

#ifndef TRACE1
#define TRACE1(x, p1)     printf("TRACE: "x", %d@%s\n", p1, __LINE__, __FILE__)
#endif

#ifndef TRACE2
#define TRACE2(x, p1, p2) printf("TRACE: "x", %d@%s\n", p1, p2, __LINE__, __FILE__)
#endif

#ifndef TRACE3
#define TRACE3(x, p1, p2, p3)     printf("TRACE: "x", %d@%s\n", p1, p2, p3, __LINE__, __FILE__)
#endif

#ifndef TRACE4
#define TRACE4(x, p1, p2, p3, p4) printf("TRACE: "x", %d@%s\n", p1, p2, p3, p4, __LINE__, __FILE__)
#endif

#ifndef TRACE5
#define TRACE5(x, p1, p2, p3, p4, p5) printf("TRACE: "x", %d@%s\n", p1, p2, p3, p4, p5, __LINE__, __FILE__)
#endif
#else

#ifndef TRACE
#define TRACE(x)
#endif

#ifndef TRACE0
#define TRACE0(x)
#endif

#ifndef TRACE1
#define TRACE1(x, p1)
#endif

#ifndef TRACE2
#define TRACE2(x, p1, p2)
#endif

#ifndef TRACE3
#define TRACE3(x, p1, p2, p3)
#endif

#ifndef TRACE4
#define TRACE4(x, p1, p2, p3, p4)
#endif

#ifndef TRACE5
#define TRACE5(x, p1, p2, p3, p4, p5)
#endif

#endif // _TRACE


#ifndef _NO_ERROR

#ifndef ERROR
#define ERROR(x)  printf("ERROR: ""%s, %d@%s\n", x, __LINE__, __FILE__)
#endif

#ifndef ERROR0
#define ERROR0(x) printf("ERROR: ""%s, %d@%s\n", x, __LINE__, __FILE__)
#endif

#ifndef ERROR1
#define ERROR1(x, p1)     printf("ERROR: "x", %d@%s\n", p1, __LINE__, __FILE__)
#endif

#ifndef ERROR2
#define ERROR2(x, p1, p2) printf("ERROR: "x", %d@%s\n", p1, p2, __LINE__, __FILE__)
#endif

#ifndef ERROR3
#define ERROR3(x, p1, p2, p3)     printf("ERROR: "x", %d@%s\n", p1, p2, p3, __LINE__, __FILE__)
#endif

#else

#ifndef ERROR
#define ERROR(x)
#endif

#ifndef ERROR0
#define ERROR0(x)
#endif

#ifndef ERROR1
#define ERROR1(x, p1)
#endif

#ifndef ERROR2
#define ERROR2(x, p1, p2)
#endif

#ifndef ERROR3
#define ERROR3(x, p1, p2, p3)
#endif

#endif // _NO_ERROR

#ifndef _NO_WARNING

#ifndef WARNING
#define WARNING(x)  printf("WARNING: ""%s, %d@%s\n", x, __LINE__, __FILE__)
#endif

#ifndef WARNING0
#define WARNING0(x) printf("WARNING: ""%s, %d@%s\n", x, __LINE__, __FILE__)
#endif

#ifndef WARNING1
#define WARNING1(x, p1)     printf("WARNING: "x", %d@%s\n", p1, __LINE__, __FILE__)
#endif

#ifndef WARNING2
#define WARNING2(x, p1, p2) printf("WARNING: "x", %d@%s\n", p1, p2, __LINE__, __FILE__)
#endif

#ifndef WARNING3
#define WARNING3(x, p1, p2, p3)     printf("WARNING: "x", %d@%s\n", p1, p2, p3, __LINE__, __FILE__)
#endif

#else

#ifndef WARNING
#define WARNING(x)
#endif

#ifndef WARNING0
#define WARNING0(x)
#endif

#ifndef WARNING1
#define WARNING1(x, p1)
#endif

#ifndef WARNING2
#define WARNING2(x, p1, p2)
#endif

#ifndef WARNING3
#define WARNING3(x, p1, p2, p3)
#endif

#endif // _NO_WARNING

#ifndef _NO_EVENT

#ifndef EVENT
#define EVENT(x)  printf("EVENT: ""%s, %d@%s\n", x, __LINE__, __FILE__)
#endif

#ifndef EVENT0
#define EVENT0(x) printf("EVENT: ""%s, %d@%s\n", x, __LINE__, __FILE__)
#endif

#ifndef EVENT1
#define EVENT1(x, p1)     printf("EVENT: "x", %d@%s\n", p1, __LINE__, __FILE__)
#endif

#ifndef EVENT2
#define EVENT2(x, p1, p2) printf("EVENT: "x", %d@%s\n", p1, p2, __LINE__, __FILE__)
#endif

#ifndef EVENT3
#define EVENT3(x, p1, p2, p3)     printf("EVENT: "x", %d@%s\n", p1, p2, p3, __LINE__, __FILE__)
#endif

#else

#ifndef EVENT
#define EVENT(x)
#endif

#ifndef EVENT0
#define EVENT0(x)
#endif

#ifndef EVENT1
#define EVENT1(x, p1)
#endif

#ifndef EVENT2
#define EVENT2(x, p1, p2)
#endif

#ifndef EVENT3
#define EVENT3(x, p1, p2, p3)
#endif

#endif // _NO_EVENT


}


// version info
#define _UTILITY_VER 1.1

#endif // _UTILITY_H
