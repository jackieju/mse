// PubFuncTable.cpp: implementation of the CPubFuncTable class.
//
//////////////////////////////////////////////////////////////////////

/************************************
  REVISION LOG ENTRY
  Revision By: Weihua Ju
  Revised on 2003-3-28 11:18:03
  Comments: 
  modify AddFunction()
  when function reloaded, show warning and rewrite this entry
  action of old version: deny to rewrite function entry
 ************************************/


#include "clib.h"
#include "os/osutils.h"
#include "PubFuncTable.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPubFuncTable::CPubFuncTable()
{
	m_iFuncNum = 0;
	m_iMaxFuncNum = 0;
	m_FuncTable = 0;
}

CPubFuncTable::~CPubFuncTable()
{
	if (this->m_FuncTable)
		delete m_FuncTable;
}

BOOL CPubFuncTable::AddFunction(long pfn, char *szName, char cParamNum)
{
	char sMsg[201] = "";

	if (szName == NULL)
		return FALSE;

	if (strlen(szName) == 0)
		return FALSE;

	// modified on 20030328 by weihua ju
	long lIndex = FindFuncByName(szName) ;
	if (lIndex >= 0)
	{
		snprintf(sMsg, 200, "SE::Warning: The pub fucntion '%s' overloaded", szName);
		cLOG(sMsg);
		//return FALSE;

		m_FuncTable[lIndex].pfn  = pfn;
		strcpy(m_FuncTable[lIndex].szName, szName); 
		m_FuncTable[lIndex].cParamNum = cParamNum;

	}
	else
	{
		// add space
		
		if (this->m_iFuncNum == this->m_iMaxFuncNum)
		{
			FUNCTIONENTRY* pTemp = new FUNCTIONENTRY[this->m_iMaxFuncNum+ FUNCNUM];
			if (pTemp == NULL)
				return FALSE;
			memset(pTemp, 0, sizeof(FUNCTIONENTRY)*(m_iMaxFuncNum+ FUNCNUM));
			if (m_FuncTable)
			{
				memcpy(pTemp, this->m_FuncTable,  this->m_iFuncNum*sizeof(FUNCTIONENTRY));
				delete this->m_FuncTable;
			}
			m_iMaxFuncNum += FUNCNUM;      
			m_FuncTable = pTemp;
		}
		
		//add
		this->m_FuncTable[m_iFuncNum].pfn  = pfn;
		strcpy(this->m_FuncTable[m_iFuncNum].szName, szName); 
		/*	this->m_FuncTable[m_iFuncNum].uMaxParamNum = uMaxParamNum;
		this->m_FuncTable[m_iFuncNum].uMinParamNum = uMinParamNum;
		*/
		this->m_FuncTable[m_iFuncNum].cParamNum = cParamNum;
		m_iFuncNum++;
	}
	return TRUE;
}


/*
   函数名称     : CPubFuncTable::FindFuncByName
   函数功能	    : 根据名字找函数信息
   变量说明     : 
   返回值       :
   -1: 没找到
   -2: 参数错误
   >=0: 函数信息数组下标
   编写人       : 居卫华
   完成日期     : 2001 - 4 - 24
*/
int CPubFuncTable::FindFuncByName(char *szName)
{
	if (szName == NULL)
		return -2;

	if (strlen(szName) == 0)
		return -2;

	for (int i= 0; i< this->m_iFuncNum; i++)
	{
		if (!strcmp(this->m_FuncTable[i].szName, szName))
			return i;
	}
	return -1;
}

/**
函数声明：	BOOL SE_AddPubFunction(HMODULE hDll, char* fnName, char* szName, char cParamNum)
函数功能：	从DLL中load函数地址到函数指针表。
参数说明：	
			[IN]HMODULE hDll	-	外部函数库
			[IN]char* fnName	-	load的函数名， 该函数名必须在外部函数库中被export出。
			[IN]char* szName	-	函数在脚本中被使用时的名称
			[IN]char cParamNum	-	函数参数个数
返 回 值：	BOOL  - 
编 写 人：	居卫华
完成日期：	2002-3-14
**/
BOOL CPubFuncTable::AddPubFunction(HMODULE hDll, char* fnName, char* szName, char cParamNum)
{
#ifndef WIN32
	FARPROC pfn = (long)dlsym((void *)hDll,fnName);
#else
	FARPROC pfn = GetProcAddress(hDll, fnName);
#endif
	//puts("reach SE_AddPubFunction");
	if (pfn == NULL)
	{
		DWORD dwError = OSGetErrorNo();
		//DWORD dwError = 0;
		printf("PS:: Get address of function '%s' failed, last error code: %d\n", fnName, dwError);
		return FALSE;
	}
	//puts("try to load g_PubFuncTable.AddFunction");
	return AddFunction((long)pfn, szName, cParamNum);
}


/**
函数声明：	long SE_LoadLib(char* szFileName)
函数功能：	从函数表load外部函数
参数说明：	
			[IN]char* szDLLName		-	外部dll
			[IN]char* szFileName	-	该dll的接口说明文件
返 回 值：	long  - 0成功, 非零失败
编 写 人：	居卫华
完成日期：	2002-3-14
**/
long CPubFuncTable::LoadLib(char *szDLLName, char* szFileName)
{
	//if (g_pPubFuncTable == NULL)

	if (szDLLName == NULL || szFileName == NULL)
		return -1;

	char szMsg[300];
	HMODULE	hDll = NULL;			// dll句柄
#ifndef WIN32

	hDll = (HMODULE)dlopen(szDLLName, RTLD_NOW/*RTLD_LAZY*/);
#else
	hDll = LoadLibrary(szDLLName);
#endif
	if (hDll == NULL)
	{
		snprintf(szMsg, 300, "SE:: Loadbrary(\"%s\") failed", szDLLName);
		nLOG(szMsg, 1);
#ifndef WIN32
		char* dlerr = dlerror();
		if (dlerr)
		{
			snprintf(szMsg, 300, "SE:: dlerror() return \"%s\")", dlerr);
			LOG(szMsg,  1);
			printf(szMsg);printf("\n");
		}
#endif
		return -2;
	}
	else
	{
		printf("SE:: LoadLib %s ok.", szDLLName);
		printf("\n");
	}
	
	m_libs.push_back((long)hDll);

	//open file
	char fnname[256];
	long paramnum = 0;
	long index;
	FILE* file;
	file = fopen(szFileName, "r");
	if (file == NULL)
	{
		unsigned long dwError = OSGetLastError();
		snprintf(szMsg, 300,"PS:: SE_LoadFunctionFile: open file '%s' failed, last error code: %d.", szFileName, dwError);
		nLOG(szMsg, 5);
		return -3;
	}
	fseek(file, 0L, SEEK_SET);

	index = 0;
	while (1)
	{
		memset(fnname, 0, 256);
		paramnum = 0;
		if (fscanf(file, "%s", fnname) == 0)
			break;
		if (fscanf(file, "%d", &paramnum) == 0)
			break;
		if (strlen(fnname) <= 0 || paramnum < 0)
			break;
		if (AddPubFunction(hDll, (char*)fnname, (char*)fnname, (char)paramnum))
		{
			snprintf(szMsg, 300,"PS:: Add external function '%s' successfully.", fnname);
			nLOG(szMsg, 9);
			index++;
		}
		else
		{
			snprintf(szMsg, 300,"PS:: Add external function '%s' failed.", fnname);
			nLOG(szMsg, 5);
		}
	}
	fclose(file);
	snprintf(szMsg, 300,"PS:: %d external function added.", index);
	nLOG(szMsg, 5);

	return 0;
}