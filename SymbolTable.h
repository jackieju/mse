
#if !defined(AFX_SYMBOLTABLE_H__66EE2870_7AFD_4B21_8C44_4D8497C6C137__INCLUDED_)
#define AFX_SYMBOLTABLE_H__66EE2870_7AFD_4B21_8C44_4D8497C6C137__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "datatype.h"

class cParser;
//符号表的表项结构
typedef struct _tagSymbolTableEle
{
	TYPEDES type;			//类型描述
	int address;			//offset
	int size_t;				//size of bytes
	char szName[42];		//name
}SYMBOLTABLEELE;
/**
	对象名称	: CSymbolTable
	对象描述	: 符号表类
	重要函数	: 

	SYMBOLTABLEELE* GetSym(char *szName);  //根据名称取符号项
	int GetSymboByAddress(int address);    //根据地址取符号的表序号
	int GetSymAddress(char* szName);       //根据名称取符号的表序号
	bool AddSymbol(char *szName, TYPEDES dt);//添加符号
	CSymbolTable();                        

	编写人		: 居卫华
	完成日期	: 2001 - 8 - 21
**/

class cParser;
class CSymbolTable  
{
public:
	int m_nMaxCount;                      //当前表的长度
	int m_nSymbolCount;                   //当前符号个数
	int m_nTotalSize;                     //当前所有符号占用的内存总数
	SYMBOLTABLEELE* tableEntry;           //符号表
	cParser* m_pParser;

	SYMBOLTABLEELE* GetSym(char *szName);  //根据名称取符号项
	int GetSymboByAddress(int address);    //根据地址取符号的表序号
	int GetSymAddress(char* szName);       //根据名称取符号的表序号
	bool AddSymbol(char *szName, TYPEDES dt);//添加符号
	CSymbolTable();                        
	virtual ~CSymbolTable();
};

#endif // !defined(AFX_SYMBOLTABLE_H__66EE2870_7AFD_4B21_8C44_4D8497C6C137__INCLUDED_)
