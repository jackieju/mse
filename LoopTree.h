// LoopTree.h: interface for the CLoopTree class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOOPTREE_H__73804CFA_1DC9_4A3A_A9DC_05883E2ACED3__INCLUDED_)
#define AFX_LOOPTREE_H__73804CFA_1DC9_4A3A_A9DC_05883E2ACED3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Function.h"
#define MAX_BREAKCONTINUE 256//最多有256个break, continue
class CLoopTree  
{
public:
	void RemoveChild();
	void AddNew(CLoopTree& newloop);
	void SetExit(int nExit);
	void SetContinue(int nContinue);
	bool AddContinue(int nContinue);
	bool AddBreak(int nBreakCmd);
	CLoopTree(CFunction* pFunc = NULL);
	virtual ~CLoopTree();

	CFunction* m_pFunc;
	int m_entry;
	int m_exit;
	CLoopTree* m_firstChild;
	CLoopTree* m_parent;
	int m_breakCmd[MAX_BREAKCONTINUE];    
	int m_continueCmd[MAX_BREAKCONTINUE];
	int m_breakNum;
	int m_continueNum;
};

#endif // !defined(AFX_LOOPTREE_H__73804CFA_1DC9_4A3A_A9DC_05883E2ACED3__INCLUDED_)
