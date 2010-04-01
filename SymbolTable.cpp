

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
����������	BOOL CSymbolTable::AddSymbol(char *szName, TYPEDES dt)
�������ܣ�	��ӷ�����
����˵����	
			[IN]char *szName	-	������
			[IN]TYPEDES dt		-	��������
�� �� ֵ��	BOOL  - �ɹ���ʧ��
�� д �ˣ�	������
������ڣ�	2001-8-21
**/
bool CSymbolTable::AddSymbol(char *szName, TYPEDES dt)
{
	int size_t = m_pParser->UnitSize(dt);
	if (size_t < 0)
		return FALSE;

	if (dt.dim>0)//������
	{
		for (int i = 0; i< dt.dim; i++)
		{
			size_t *= dt.dimsize[i];
		}
	}
/*	int size16;//��16bit�ֶ����Ĵ�С
	size16 = size_t + (4 - size_t%4);
*/
	int size16 = size_t;//���Ҫ��16bit����Ļ�, ģ�庯��������ʱ�����Ĵ���Ҳ���밴16bit����, ����������鷳, �������ڲ�����
	if (szName != NULL)
	{
		//�����Ƿ���ض���
		for (int i = 0; i< this->m_nSymbolCount; i++)
		{
			if (!strcmp(this->tableEntry[i].szName, szName))
			{
				REPORT_COMPILE_ERROR("symbol redefined\r\n");
				return false;
			}
		}
	}
	//������鲻����
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
����������	int CSymbolTable::GetSymAddress(char *szName)
�������ܣ�	���ݷ�������ȥ���ŵ�ַ
����˵����	
			[IN]char *szName	-	��������
�� �� ֵ��	int  - �ɹ���ʧ��
�� д �ˣ�	������
������ڣ�	2001-8-21
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
����������	int CSymbolTable::GetSymboByAddress(int address)
�������ܣ�	���ݵ�ַȡ�������ڷ��ű������
����˵����	
			[IN]int address	-	��ַ
�� �� ֵ��	int  - �ɹ���ʧ��
�� д �ˣ�	������
������ڣ�	2001-8-21
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
����������	SYMBOLTABLEELE* CSymbolTable::GetSym(char* szName)
�������ܣ�	���ݷ�����ȥ������Ϣ�ṹ
����˵����	
			[IN]char* szName	-	������
�� �� ֵ��	SYMBOLTABLEELE*  - ������Ϣ�ṹ
�� д �ˣ�	������
������ڣ�	2001-8-21
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
