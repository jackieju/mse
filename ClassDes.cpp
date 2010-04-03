#include "clib.h"
#include "os/CSS_LOCKEX.h"
#include "memory.h"
#include "ScriptFuncTable.h"

#include "PubFuncTable.h"
#include "cocor/cp.hpp"
#include "ClassDes.h"


CClassDes::CClassDes(cParser* p)
{
	//m_lMemberNum = 0;
	//m_lTableSize = 0;
	
	memset(this->m_szName, 0, 1024);
	memset(this->szParentName, 0, 1024);
	m_pParser = p;

	//m_MemberTable = NULL;
	//m_lCurSize = 0;

	//CFunction* pfn = new CFunction();
	//pfn->AddCommand
	//this->fnTable.AddFunction(pfh);

}

CClassDes::~CClassDes(void)
{
		/*if (m_MemberTable)
		delete m_MemberTable;*/
}

/*BOOL CClassDes::AddMember(char *szName, TYPEDES& dt)
{
	if (strlen(szName) > 20)
	{
		nLOG("member name too long", 300);
		return FALSE;
	}
	//检查是否重定义
	for (int i= 0; i< this->m_lMemberNum; i++)
	{
		if (!strcmp(this->m_MemberTable[i].szName, szName))
		{
			nLOG("Class member redefined", 300);
			return FALSE;
		}
	}

	if (this->m_lMemberNum == this->m_lTableSize)
	{
		OBJMEMDES* pNewTable = NULL;
		
		pNewTable = new OBJMEMDES[m_lTableSize+50];
		if (pNewTable == NULL)
		{
			REPORT_MEM_ERROR("Memory allocation failed\r\n");
			return FALSE;
		}
		memcpy(pNewTable, this->m_MemberTable, sizeof(OBJMEMDES)*m_lMemberNum);
		delete m_MemberTable;
		this->m_MemberTable = pNewTable;		
		m_lTableSize += 50;
	}
	m_MemberTable[m_lMemberNum].offset = this->m_lCurSize;
	long size = m_pParser->UnitSize(dt);
	if (dt.dim > 0)
	{
		for (int i= 0 ; i< dt.dim; i++)
		{
			size *= dt.dimsize[i];
		}
	}
	m_lCurSize += size;

	strcpy(m_MemberTable[m_lMemberNum].szName, szName);
	memcpy(&m_MemberTable[m_lMemberNum].dt, &dt, sizeof(TYPEDES));
	this->m_lMemberNum++;
	return TRUE;
}

*/

BOOL CClassDes::SetName(char* szName)
{
	if (strlen(m_szName) > 1024)
	{
		nLOG("member name too long", 300);
		return FALSE;
	}
	strcpy(m_szName, szName);
	return TRUE;		
}

/*
OBJMEMDES *CClassDes::GetMemberByName(char* szName)
{
	// find
	for (int i= 0; i< this->m_lMemberNum; i++)
	{
		if (!strcmp(this->m_MemberTable[i].szName, szName))
		{
			return &m_MemberTable[i];
		}
	}
	return NULL;
}
*/