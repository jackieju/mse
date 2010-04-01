// LoopTree.cpp: implementation of the CLoopTree class.
//
//////////////////////////////////////////////////////////////////////

//#include "StdAfx.h"
//#include "windows.h"
#include "clib.h"
#include "Function.h"
#include "LoopTree.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoopTree::CLoopTree(CFunction* pFunc)
{
	m_entry = m_exit = -1;
	m_firstChild = m_parent = NULL;
	m_breakNum = m_continueNum = 0;
	m_pFunc = pFunc;	
}

CLoopTree::~CLoopTree()
{
/*	if (this->m_parent != NULL)
	{
		if (m_parent->m_firstChild == this)
			m_parent->m_firstChild = NULL;
	}
*/
	if (m_firstChild)
	{
		delete m_firstChild;
		m_firstChild = NULL;
	}
}

bool CLoopTree::AddBreak(int nBreakCmd)
{
	if (m_breakNum >= MAX_BREAKCONTINUE)
	{	
		return FALSE;
	}
	this->m_breakCmd[this->m_breakNum] = nBreakCmd;
	this->m_breakNum++;
	return TRUE;
}

bool CLoopTree::AddContinue(int nContinue)
{
	if (m_breakNum >= MAX_BREAKCONTINUE)
	{	
		return FALSE;
	}
	this->m_continueCmd[this->m_continueNum] = nContinue;
	this->m_continueNum++;
	return TRUE;
}


/*
   函数名称     : CLoopTree::SetExit
   函数功能	    : 设置出口并回写所有的break和continue语句
   变量说明     : 
   返回值       : 
   编写人       : 居卫华
   完成日期     : 2001 - 4 - 23
*/
void CLoopTree::SetExit(int nExit)
{
	int i;
	this->m_exit = nExit;

	for (i = 0; i< this->m_breakNum; i++)
	{
		this->m_pFunc->m_pCmdTable[this->m_breakCmd[i]].op[0] = nExit;
	}
}
void CLoopTree::SetContinue(int nContinue)
{	

	int i;
	for (i = 0; i< this->m_continueNum; i++)
	{
		this->m_pFunc->m_pCmdTable[this->m_continueCmd[i]].op[0] = nContinue ;
	}
}

void CLoopTree::AddNew(CLoopTree& newloop)
{
	if (this->m_firstChild == NULL)
		this->m_firstChild = &newloop;
	newloop.m_parent = this;
}


void CLoopTree::RemoveChild()
{
	if (m_firstChild == NULL)
		return;
	delete this->m_firstChild;
	this->m_firstChild = NULL;
}
