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
    char dataType;       //����������	
	long unitsize;    //���ݵĵ�λ����
	char arraydim;       //����ά��(�����ά)
	long  arraysize[4];   //���鳤��
	unsigned char* content;       //����	
	int   contentSize;   //���ݵĳ���
}PARAM;

typedef struct _tagPARAMHDR
{
    char dataType;       //����������	
	long unitsize;    //���ݵĵ�λ����
	char arrayDim;       //����ά��(�����ά)
	int  arraySize[4];   //���鳤��		'
	unsigned char *content;
}PARAMHDR;
#pragma pack()


void LogFunc(char* msg, char* file, long line, long level);
long __stdcall SE_ExecScript1(long req, long lWorkMode, long lFlag);
long __stdcall SE_ExecScript2(long req, long lWorkMode, long lFlag);

#endif //__COMPILER_H__
