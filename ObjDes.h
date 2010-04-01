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
	TYPEDES dt;				//��������
	char szName[64];		//����
	long offset;			//offset
}OBJMEMDES;
class cParser;
class CObjDes  
{
private:
	OBJMEMDES *m_MemberTable;	//��Ա�б�
	long m_lTableSize;			//buffer��С
	long m_lMemberNum;			//��Ա��
	char m_szName[21];			//��obj������
	long m_lCurSize;				//��ǰobj�Ĵ�С

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
