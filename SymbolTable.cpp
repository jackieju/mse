

#include "clib.h"
//#include "windows.H"
#include "PubFuncTable.h"
#include "cocor/cp.hpp"
#include "SymbolTable.h"
#include "os/CSS_LOCKEX.H"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSymbolTable::CSymbolTable()
{
	this->m_nMaxCount = 64;
	this->m_nSymbolCount = 0;
	this->m_nTotalSize = 0;
	this->tableEntry = new SYMBOLTABLEELE[64];
	m_pParser = NULL;

}

CSymbolTable::~CSymbolTable()
{
	if (tableEntry != NULL)
		delete tableEntry;
}


/**
函数声明：	BOOL CSymbolTable::AddSymbol(char *szName, TYPEDES dt)
函数功能：	添加符号项
参数说明：	
			[IN]char *szName	-	符号名
			[IN]TYPEDES dt		-	类型描述
返 回 值：	BOOL  - 成功或失败
编 写 人：	居卫华
完成日期：	2001-8-21
**/
bool CSymbolTable::AddSymbol(char *szName, TYPEDES dt)
{
	int size_t = m_pParser->UnitSize(dt);
	if (size_t < 0)
		return FALSE;

	if (dt.dim>0)//是数组
	{
		for (int i = 0; i< dt.dim; i++)
		{
			size_t *= dt.dimsize[i];
		}
	}
/*	int size16;//按16bit字对齐后的大小
	size16 = size_t + (4 - size_t%4);
*/
	int size16 = size_t;//如果要按16bit对齐的话, 模板函数被调用时参数的传入也必须按16bit对齐, 这样会造成麻烦, 所以现在不对齐
	if (szName != NULL)
	{
		//查找是否会重定义
		for (int i = 0; i< this->m_nSymbolCount; i++)
		{
			if (!strcmp(this->tableEntry[i].szName, szName))
			{
				REPORT_COMPILE_ERROR("symbol redefined\r\n");
				return false;
			}
		}
	}
	//如果数组不够长
	if (this->m_nSymbolCount == this->m_nMaxCount)
	{//if full
		SYMBOLTABLEELE* pNew = new SYMBOLTABLEELE[this->m_nMaxCount + 64];
		if (pNew == NULL)
		{
			REPORT_MEM_ERROR("interal error: can not allocate more symbol table");
			return false;
		}
		memcpy(pNew, this->tableEntry, this->m_nMaxCount*sizeof(SYMBOLTABLEELE));
		delete this->tableEntry;
		tableEntry = pNew;
		m_nMaxCount += 64;
	}
	//add symbol
	this->tableEntry[this->m_nSymbolCount].address = this->m_nTotalSize;//address
	tableEntry[this->m_nSymbolCount].size_t = size_t;                   //size
	memcpy(&(tableEntry[this->m_nSymbolCount].type), &dt, sizeof(TYPEDES));//datatype

	//symbol name
	if (szName != NULL)
		strcpy(tableEntry[this->m_nSymbolCount].szName, szName);
	else
		tableEntry[this->m_nSymbolCount].szName[0] = 0;

	this->m_nTotalSize += size16;
	m_nSymbolCount++;
	return true;
}


/**
函数声明：	int CSymbolTable::GetSymAddress(char *szName)
函数功能：	根据符号名称去符号地址
参数说明：	
			[IN]char *szName	-	符号名称
返 回 值：	int  - 成功或失败
编 写 人：	居卫华
完成日期：	2001-8-21
**/
int CSymbolTable::GetSymAddress(char *szName)
{
	if (szName == NULL)
		return FALSE;

	int i;
	for ( i = 0; i< this->m_nSymbolCount; i++)
	{
		if (!strcmp(this->tableEntry[i].szName, szName))
		{			
			return tableEntry[i].address;
		}
	}
	return -1;

}


/**
函数声明：	int CSymbolTable::GetSymboByAddress(int address)
函数功能：	根据地址取符号再在符号表中序号
参数说明：	
			[IN]int address	-	地址
返 回 值：	int  - 成功或失败
编 写 人：	居卫华
完成日期：	2001-8-21
**/
int CSymbolTable::GetSymboByAddress(int address)
{
	int i;
	for (i = 0; i< this->m_nSymbolCount; i++)
	{
		if (this->tableEntry[i].address == address)
			return i;
	}
	return -1;
}


/**
函数声明：	SYMBOLTABLEELE* CSymbolTable::GetSym(char* szName)
函数功能：	根据符号名去符号信息结构
参数说明：	
			[IN]char* szName	-	符号名
返 回 值：	SYMBOLTABLEELE*  - 符号信息结构
编 写 人：	居卫华
完成日期：	2001-8-21
**/
SYMBOLTABLEELE* CSymbolTable::GetSym(char* szName)
{
	if (szName == NULL)
		return FALSE;

	int i;
	for ( i = 0; i< this->m_nSymbolCount; i++)
	{
		if (!strcmp(this->tableEntry[i].szName, szName))
		{			
			return &tableEntry[i];
		}
	}
	return NULL;
}
