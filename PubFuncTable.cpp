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
   ��������     : CPubFuncTable::FindFuncByName
   ��������	    : ���������Һ�����Ϣ
   ����˵��     : 
   ����ֵ       :
   -1: û�ҵ�
   -2: ��������
   >=0: ������Ϣ�����±�
   ��д��       : ������
   �������     : 2001 - 4 - 24
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
����������	BOOL SE_AddPubFunction(HMODULE hDll, char* fnName, char* szName, char cParamNum)
�������ܣ�	��DLL��load������ַ������ָ���
����˵����	
			[IN]HMODULE hDll	-	�ⲿ������
			[IN]char* fnName	-	load�ĺ������� �ú������������ⲿ�������б�export����
			[IN]char* szName	-	�����ڽű��б�ʹ��ʱ������
			[IN]char cParamNum	-	������������
�� �� ֵ��	BOOL  - 
�� д �ˣ�	������
������ڣ�	2002-3-14
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
����������	long SE_LoadLib(char* szFileName)
�������ܣ�	�Ӻ�����load�ⲿ����
����˵����	
			[IN]char* szDLLName		-	�ⲿdll
			[IN]char* szFileName	-	��dll�Ľӿ�˵���ļ�
�� �� ֵ��	long  - 0�ɹ�, ����ʧ��
�� д �ˣ�	������
������ڣ�	2002-3-14
**/
long CPubFuncTable::LoadLib(char *szDLLName, char* szFileName)
{
	//if (g_pPubFuncTable == NULL)

	if (szDLLName == NULL || szFileName == NULL)
		return -1;

	char szMsg[300];
	HMODULE	hDll = NULL;			// dll���
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