
#if !defined(AFX_SYMBOLTABLE_H__66EE2870_7AFD_4B21_8C44_4D8497C6C137__INCLUDED_)
#define AFX_SYMBOLTABLE_H__66EE2870_7AFD_4B21_8C44_4D8497C6C137__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "datatype.h"

class cParser;
//���ű�ı���ṹ
typedef struct _tagSymbolTableEle
{
	TYPEDES type;			//��������
	int address;			//offset
	int size_t;				//size of bytes
	char szName[42];		//name
}SYMBOLTABLEELE;
/**
	��������	: CSymbolTable
	��������	: ���ű���
	��Ҫ����	: 

	SYMBOLTABLEELE* GetSym(char *szName);  //��������ȡ������
	int GetSymboByAddress(int address);    //���ݵ�ַȡ���ŵı����
	int GetSymAddress(char* szName);       //��������ȡ���ŵı����
	bool AddSymbol(char *szName, TYPEDES dt);//��ӷ���
	CSymbolTable();                        

	��д��		: ������
	�������	: 2001 - 8 - 21
**/

class cParser;
class CSymbolTable  
{
public:
	int m_nMaxCount;                      //��ǰ��ĳ���
	int m_nSymbolCount;                   //��ǰ���Ÿ���
	int m_nTotalSize;                     //��ǰ���з���ռ�õ��ڴ�����
	SYMBOLTABLEELE* tableEntry;           //���ű�
	cParser* m_pParser;

	SYMBOLTABLEELE* GetSym(char *szName);  //��������ȡ������
	int GetSymboByAddress(int address);    //���ݵ�ַȡ���ŵı����
	int GetSymAddress(char* szName);       //��������ȡ���ŵı����
	bool AddSymbol(char *szName, TYPEDES dt);//��ӷ���
	CSymbolTable();                        
	virtual ~CSymbolTable();
};

#endif // !defined(AFX_SYMBOLTABLE_H__66EE2870_7AFD_4B21_8C44_4D8497C6C137__INCLUDED_)
