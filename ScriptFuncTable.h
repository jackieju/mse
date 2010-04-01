// ScriptFuncTable.h: interface for the CScriptFuncTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIPTFUNCTABLE_H__29ABC564_3776_4270_B2F1_DF81B4F50B1F__INCLUDED_)
#define AFX_SCRIPTFUNCTABLE_H__29ABC564_3776_4270_B2F1_DF81B4F50B1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "log.h"
#include "commondef.h"
//#include "Function.h"
//#include "utility.h"
#include "os/css_lockex.h"

class CFunction;
#define SCRIPTFUNCTABLEINCSIZE 20

class CScriptFuncTable  
{
public:
	bool Clear(long lTime);
	bool m_bOverWrite;
	long SetFunctionMode(char* szFuncID, long lMode);
	void ReleaseFunc();
	CFunction* GetFunction(char* szID, long* index);
		CFunction* GetFunction(long index){
	//lock
		if (ReadLock() != LOCKEX_ERR_OK)
		{
			REPORT_ERROR("ReadLock failed", 9);
			return NULL;
		}
		CFunction* ret = this->table[index];
		ReadUnlock();
		return ret;
	};
	bool AddFunction(CFunction* pfn);
	CScriptFuncTable();
	virtual ~CScriptFuncTable();
	int m_FuncNum;
private:
	//CSingleWriteMultiRead m_lock;
 	CSS_SingleWriteMultiRead	m_lock;
	CFunction** table;
	int m_tableSize;
	void WriteUnlock();
	void ReadUnlock();
	DWORD WriteLock();
	DWORD ReadLock();
};

#endif // !defined(AFX_SCRIPTFUNCTABLE_H__29ABC564_3776_4270_B2F1_DF81B4F50B1F__INCLUDED_)
