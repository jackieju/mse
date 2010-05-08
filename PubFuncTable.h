// PubFuncTable.h: interface for the CPubFuncTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUBFUNCTABLE_H__E9E34579_3E3D_40FF_8CFE_2404A7E276C5__INCLUDED_)
#define AFX_PUBFUNCTABLE_H__E9E34579_3E3D_40FF_8CFE_2404A7E276C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define MAX_FUNCNAME_LENGTH 128
#include <map>

//存放公用函数信息的结构
typedef struct _tagFunctionEntry
{
	#ifdef WIN32
	FARPROC pfn;                       //函数指针
	#else
	void* pfn;
	#endif 
	char szName[MAX_FUNCNAME_LENGTH];  //函数名
	/*unsigned int uMaxParamNum;	       //最多参数个数 
	unsigned int uMinParamNum;         //最少参数个数*/
	//暂时规定所有的函数参数个数必须一定, 即不支持参数个数不定的函数
	char cParamNum;                    //参数个数
}FUNCTIONENTRY;
#define FUNCNUM 20

class CPubFuncTable  
{
	int m_iFuncNum;
	int m_iMaxFuncNum;
	std::map<std::string, long>	m_libs;


public:
	CPubFuncTable();
	virtual ~CPubFuncTable();
	FUNCTIONENTRY *m_FuncTable;	
	
	int FindFuncByName(char* szName);
	BOOL AddFunction(void* pfn, char* szName, char cParamNum);
	BOOL AddPubFunction(HMODULE hDll, char* fnName, char* szName, char cParamNum);
	long LoadLib(char *szDLLName, char* szFileName);
	
};

#endif // !defined(AFX_PUBFUNCTABLE_H__E9E34579_3E3D_40FF_8CFE_2404A7E276C5__INCLUDED_)
