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

#include <vector>
typedef std::vector<std::string>			string_array;



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

#ifndef MIN 
#define MIN(a,b) (((a)<(b)) ? (a) : (b))
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
namespace JUJU{
void QSort1(char *a, size_t n, size_t es, long (*cmp)(char*, char*, long, long, long), long p1, long p2, long p3);
void QSort2(char *a, size_t n, size_t es, long (*cmp)(char*, char*, long, long, long, long, long, long), long p1, long p2, long p3, long q1, long q2, long q3);
bool CheckPath(const char* strDir);
#ifdef WIN32
//bool IsFileExist(char* fileName, WIN32_FIND_DATA &FileInfo);
#define beep() Beep(1000,500) 
#else
//#include <curses.h>
#endif
void SystemLog(char* filename, char* msg, char* sourcefile, long line, char* format = "%s, \tsource=%s, \tline=%d\n");

// create a string with speiciifed string, the return string must be deleted by caller
// the demerit of this function is:
// he will allocate buffer on heap, so maybe slow than other string function
#ifdef WIN32
char* __stdcall    CreateFormatString(long init_size, char* format, ...);
#endif
void* DllNew(size_t size);
template <class LIST, class S, class E>
#ifdef WIN32
long binary_search(LIST array, long start, S s, long number, int (__cdecl* compare)(S, E), bool& bFound);
#else

long binary_search(LIST array, long start, S s, long number, int (* compare)(S, E), bool& bFound);

#endif
void DllDelete(unsigned char* p );
unsigned char* ReadFile(char* szFileName, long &len);
void WriteFile(char* filename, unsigned char* buffer, long &len, char* flag = "wb");
void DeleteStr(char* p );
void utoa(wchar_t* uc, char* str, int max_len);
char* utoa(wchar_t* uc);
void atou(char* pstr, wchar_t* jstr_u, int max_len);
wchar_t* atou(char* pstr);

/* string operation */


std::string  getFileName(char* fullName);	// get file name excluding extension
std::string  getFileExt(char* fullName);	// get file extension name
std::string  getFilePath(char* fullName);	// get file path, /usr/abc -> /usr/
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
	char* getWorkingPath();

}
#endif // _UTILITY_H
