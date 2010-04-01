

#include "clib.h"
#include "os/CSS_LOCKEX.h"
//#include "windows.h"
#include "assert.h"
#include "Function.h"
#include "ScriptFuncTable.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScriptFuncTable::CScriptFuncTable()
{
	m_FuncNum = 0;
	m_tableSize = 0;
	table = NULL;
	m_bOverWrite = TRUE;
	m_lock.Create();
}

CScriptFuncTable::~CScriptFuncTable()
{	
	for (int i = 0; i < this->m_FuncNum; i ++)
	{
		if (table[i] != NULL)
		{
			delete table[i];
			table[i] = NULL;
		}
	}
	if (table) 
	{
		delete table;
		table = NULL;
	}
}



/*
函数声明：	BOOL CScriptFuncTable::AddFunction(CFunction *pfn)
函数功能：	添加新的funciton, 并按字母排序
参数说明：	
			[IN]CFunction *pfn	-	要添加的函数指针
返 回 值：	BOOL  - 成功或失败
编 写 人：	居卫华
完成日期：	2001-6-26
*/
bool CScriptFuncTable::AddFunction(CFunction *pfn)
{
	char szMsg[1000];
	BOOL bFound = FALSE;
	if (pfn == NULL)
		return false;
	//lock
	//if (WriteLock() != WAIT_OBJECT_0)
	if (WriteLock() != LOCKEX_ERR_OK)
	{
		REPORT_ERROR("WriteLock failed", 50);
		return false;
	}
	sprintf(szMsg, "scipt table write lock ok");
	nLOG(szMsg, 200);

	int index = 0;
	int k;
	if (m_FuncNum > 0)
	//check whether there is a function with the same name
	{
		int i, j, l;
		
		i = 0;
		j = this->m_FuncNum-1;
		k = -1;
		char* szName;
		
		while (i <= j)
		{
			k = (i+j)/2;
			szName = table[k]->m_szName;
			l = strcmp(pfn->m_szName, szName);
			if (l == 0)
			{
				index = k;
				bFound = TRUE;
				break;
			}
			if (l < 0)
				j = k - 1;
			if (l > 0)
				i = k + 1;
		}	
		if (!bFound)
		{
			if ( l > 0)
				index = k+1;
			if ( l < 0)
				index = k;
		}
	}

	if (bFound)
	{
		if (m_bOverWrite)
		{
			//write over the function
			delete table[index];
			table[index] =  pfn;
			WriteUnlock();
			sprintf(szMsg, "function <%s> is written over", pfn->m_szName);
			REPORT_ERROR(szMsg, 1);
			return true;
		}
		else
		{
			WriteUnlock();
			sprintf(szMsg, "function <%s> is redefined", pfn->m_szName);
			REPORT_ERROR(szMsg, 1);
			return false;
		}
	}
	////////////////////////////////////////////////

	//check whether have enough space
	if (!bFound && m_FuncNum == m_tableSize)
	{
		CFunction **pNew = new CFunction*[m_tableSize + SCRIPTFUNCTABLEINCSIZE];
		if (pNew == NULL)
		{
			WriteUnlock();
			return false;
		}
		memset(pNew, 0, (m_tableSize + SCRIPTFUNCTABLEINCSIZE)*sizeof(CFunction*));
		if (m_FuncNum > 0)
			memcpy(pNew, table, m_FuncNum*sizeof(CFunction*));
		if (table)
			delete table;
		table = pNew;
		m_tableSize += SCRIPTFUNCTABLEINCSIZE;
	}
/*	////////////////////////////////////////////////
	//for test
	printf("------------------------------\n");
	printf("NO	  function name		address\n");
	for (int i =0; i<m_FuncNum; i++)
	{
		printf("%4d: %s	%x\n", i, this->table[i]->m_szName, table[i]);
	}
//	printf("index: %d, m_FuncNum: %d, m_tableSize: %d\n", index, m_FuncNum, this->m_tableSize);
	printf("dest: %x, source: %x, size: %d, pfn: %x\n", &table[index+1], &table[index], (m_FuncNum - index)*sizeof(CFunction*), pfn);
	////////////////////////////////////////////////
*/
	//insert
	long lCopySize = (m_FuncNum - index)*sizeof(CFunction*);
	BYTE * pMid = new BYTE[lCopySize];
	if (pMid == NULL)
	{
		sprintf(szMsg, "allocate memory failed");
		nLOG(szMsg, 1);
		WriteUnlock();
		return FALSE;

	}
	memcpy(pMid, &(table[index]), lCopySize);
	memcpy(&(table[index+1]), pMid, lCopySize); 
	table[index] = pfn;

	delete pMid;
	
/*	printf("dest: %x, source: %x, size: %d, pfn: %x\n", &table[index+1], &table[index], (m_FuncNum - index)*sizeof(CFunction*), pfn);
*/	
	m_FuncNum++;
	//unlock
	WriteUnlock();
	sprintf(szMsg, "Add the %dth function <%s> ok", m_FuncNum, pfn->m_szName);
	nLOG(szMsg, 9);

/*	////////////////////////////////////////////////
	//for test
	printf("NO	  function name		address\n");
	for ( i =0; i<m_FuncNum; i++)
	{
		printf("%4d: %s	%x\n", i, this->table[i]->m_szName, table[i]);
	}
//	printf("index: %d, m_FuncNum: %d, m_tableSize: %d\n", index, m_FuncNum, this->m_tableSize);
	printf("------------------------------\n");
	////////////////////////////////////////////////
*/
	return true;
}

DWORD CScriptFuncTable::ReadLock()
{
	return m_lock.WaitToRead(10000);
}

DWORD CScriptFuncTable::WriteLock()
{
	return m_lock.WaitToWrite(10000);
}

void CScriptFuncTable::ReadUnlock()
{
	m_lock.DoneReading();
}

void CScriptFuncTable::WriteUnlock()
{
	m_lock.DoneWriting();
}


/*
   函数名称     : CScriptFuncTable::GetFunction
   函数功能	    : 根据ID去函数, 目前id就是函数名, 用二分法查找
   变量说明     :
   返回值       :
   编写人       : 居卫华
   完成日期     : 2001 - 5 - 14
*/
CFunction* CScriptFuncTable::GetFunction(char *szID, long* index)
{
	*index = -1;

	int i, j, k, l;

	i = 0;
	j = this->m_FuncNum-1;
	k = -1;

	char* szName;
	//printf("k=%d l=%d j=%d i=%d\n", k, l, j, i);
	//lock
	if (ReadLock() != LOCKEX_ERR_OK)
	{
		REPORT_ERROR("ReadLock failed", 9);
		return NULL;
	}
	while (i <= j)
	{
		//printf("k=%d l=%d j=%d i=%d\n", k, l, j, i);

		k = (i+j)/2;
		szName = table[k]->m_szName;

		//printf("szID:%s name:%s",szID, szName);

		l = strcmp(szID, szName);
		if (l == 0)
		{
			*index = k;
			return table[k];
		}
		else
		{
			if (l < 0)//table[i] < table[k]
				j = k -1;
			if (l > 0)//table[i] > table[k]
				i = k + 1;
		}
/*		printf("k=%d l=%d j=%d i=%d\n", k, l, j, i);*/
	}
	//unlock
	ReadUnlock();
	return NULL;
}

void CScriptFuncTable::ReleaseFunc()
{
	ReadUnlock();
}


/*
   函数名称     : CScriptFuncTable::SetFunctionMode
   函数功能	    : 设置某个function的运行模式
   变量说明     : 
   返回值       : >=0 该函数原来的模式; <0 找不到该函数
   编写人       : 居卫华
   完成日期     : 2001 - 5 - 18
*/
long CScriptFuncTable::SetFunctionMode(char* szFuncID, long lMode)
{
	long index = 0;
	long lOldMode = 0;
	CFunction* pfn = GetFunction(szFuncID, &index);
	if (pfn == NULL)
	{
		return -1;
	}
	lOldMode = pfn->m_nWorkMode;
	pfn->m_nWorkMode = lMode;
	return lOldMode;	
}


/*
函数声明：	BOOL CScriptFuncTable::Clear(long lTime)
函数功能：	释放表空间
参数说明：	
			[IN]long lTime	-	等待的最大时间
返 回 值：	BOOL  - 
编 写 人：	居卫华
完成日期：	2001-7-6
*/
bool CScriptFuncTable::Clear(long lTime)
{

	if (m_lock.WaitToWrite(lTime) != LOCKEX_ERR_OK)
	{
		return false;
	}

	//clear
	for (int i = 0; i < this->m_FuncNum; i ++)
	{
		if (table[i] != NULL)
		{
			delete table[i];
			table[i] = NULL;
		}
	}
	if (table) 
	{
		delete table;
		table = NULL;
	}

	m_FuncNum = 0;
	m_tableSize = 0;
	table = NULL;
	m_bOverWrite = TRUE;
	WriteUnlock();
	return true;
}

