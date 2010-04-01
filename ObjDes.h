// ObjDes.h: interface for the CObjDes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJDES_H__E44F0C3A_50BE_4A41_A6EB_0FF96E86CE57__INCLUDED_)
#define AFX_OBJDES_H__E44F0C3A_50BE_4A41_A6EB_0FF96E86CE57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "datatype.h"

typedef struct _tagObjMemDes
{
	TYPEDES dt;				//数据类型
	char szName[64];		//名称
	long offset;			//offset
}OBJMEMDES;
class cParser;
class CObjDes  
{
private:
	OBJMEMDES *m_MemberTable;	//成员列表
	long m_lTableSize;			//buffer大小
	long m_lMemberNum;			//成员数
	char m_szName[21];			//该obj的名称
	long m_lCurSize;				//当前obj的大小

	cParser *m_pParser;

public:
	CObjDes(cParser* p);
	virtual ~CObjDes();
	BOOL AddMember(char* szName, TYPEDES& dt);
	char*  GetObjName();
	BOOL SetName(char* szName);
	long GetSize();
	OBJMEMDES *GetMemberByName(char* szName);

};

#endif // !defined(AFX_OBJDES_H__E44F0C3A_50BE_4A41_A6EB_0FF96E86CE57__INCLUDED_)
