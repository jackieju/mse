#ifndef __COMPILER_H__
#define __COMPILER_H__
#include <string.h>
#include "clib.h"
#include "cocoR/CR_ERROR.hpp"
#include "cocoR/cp.hpp"

#define SCRIPTTABLE_NUM 10
class MyError : public CRError {
public:
//    MyError(){};
    MyError(char *name, AbsScanner *S) : CRError(name, S, MAXERROR) {};
    virtual char *GetUserErrorMsg(int n);
    virtual char *GetErrorMsg(int n)
	{ if (n <= MAXERROR) return ErrorMsg[n];
	else return GetUserErrorMsg(n);
	};
	
	void Init(char *name, int MinUserNo = MINERRORNO, int MinErr = 2)
	{
		strcpy(FileName, name);
		lst = stderr;
		Errors = 0; FirstErr = LastErr = NULL; ErrorDist = MinErr;
		MinErrorDist = MinErr;
		MinUserError = MinUserNo;
	}
	
private:
    static char *ErrorMsg[];
};

class CCompiler
{
public:
	CCompiler();
	~CCompiler();
//static  CScriptFuncTable functable[SCRIPTTABLE_NUM];
//static long g_lActiveScriptTable;
	// table of classes, classes
	static CClassDesTable classTable;
	// external native function (print, log, ...)
	static CPubFuncTable m_PubFuncTable;
	// compile source file
	BOOL Compile(char *szFileName);
	void setOutput(char* szFileName){
		if (szFileName == NULL)
			return;
		strcpy(m_szErrFile, szFileName);
	}
	
	char* getCurSrcFile(){
		return m_szSourceFile;
	}
protected:
	
private:
	//MyError m_Error;
	//cParser m_Parser;
	//cScanner m_Scanner;

	static char m_szErrMsg[1024];
	static char m_szErrFile[_MAX_PATH];
	char m_szSourceFile[_MAX_PATH];
};


#define CM_NOTOVERWRITE 1 //not write over the function with the same name, and return FALSE
#pragma pack(1)
typedef struct _tagPARAM
{
    char dataType;       //参数的类型	
	long unitsize;    //数据的单位长度
	char arraydim;       //数组维数(最大四维)
	long  arraysize[4];   //数组长度
	unsigned char* content;       //内容	
	int   contentSize;   //内容的长度
}PARAM;

typedef struct _tagPARAMHDR
{
    char dataType;       //参数的类型	
	long unitsize;    //数据的单位长度
	char arrayDim;       //数组维数(最大四维)
	int  arraySize[4];   //数组长度		'
	unsigned char *content;
}PARAMHDR;
#pragma pack()


void LogFunc(char* msg, char* file, long line, long level);
long __stdcall SE_ExecScript1(long req, long lWorkMode, long lFlag);
long __stdcall SE_ExecScript2(long req, long lWorkMode, long lFlag);

#endif //__COMPILER_H__
