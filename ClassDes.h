#ifndef __CLASSDES_H__
#define __CLASSDES_H__
#include "datatype.h"
#include "ScriptFuncTable.h"
#include "SymbolTable.h"
#include "function.h"
#include <string>

typedef struct _tagObjMemDes
{
	TYPEDES dt;				//数据类型
	char szName[64];		//名称
	long offset;			//offset
}OBJMEMDES;
class cParser;
class CClassDes{

private:

	// primitive members
	//OBJMEMDES *m_MemberTable;	// 成员列表
	//long m_lTableSize;			// buffer大小
	//long m_lMemberNum;			// 成员数
	//long m_lCurSize;			// 当前obj的大小

	CSymbolTable m_memberTable;
	cParser *m_pParser;

	char m_szName[1024]; // full class name
	CScriptFuncTable fnTable; // member function table
	char szParentName[1024]; // full class name of parent

public:
	CScriptFuncTable* getFuncTable(){
		return &fnTable;
	}
	CFunction* getMethod(char* szName){
		long index;
		return fnTable.GetFunction(szName, &index);

	}
	char* GetFullName(){
		return m_szName;
	}
	char* getClassName(){
		// TODO
		return m_szName;
	}

	char* getPackageName(){
		// TODO
		return m_szName;
	}

	// add primitive member
	bool AddMember(char* szName, TYPEDES& dt){
		return this->m_memberTable.AddSymbol(szName, dt);
	};
	BOOL SetName(char* szName);
	long GetMemberNum(){
		return m_memberTable.m_nSymbolCount;
	};
	void setParent(char* szName){
		strcpy(szParentName, szName);
	}
	SYMBOLTABLEELE *GetMemberByName(char* szName){
		return  this->m_memberTable.GetSym(szName);
	};
	std::string* output(){
		char msg[1024] = "";
		std::string *ret = new std::string("");
		std::string &r = *ret;
		r += "Class";
		r += this->GetFullName() ;
		r += "\r\n";
		r += "member:\r\n===========================\r\n";
		r += "name\ttype\taddress\tsize\t\r\n";



		for (int i = 0; i< m_memberTable.m_nSymbolCount; i ++){
			snprintf(msg, 1000, "%s\t%04x\t%04x\t%04x\r\n", m_memberTable.tableEntry[i].szName, 
				m_memberTable.tableEntry[i].type.type, m_memberTable.tableEntry[i].address, m_memberTable.tableEntry[i].size_t);
			r += msg;
		}		
		r += "===========================\r\n\r\n";
		r += "method:\r\n===========================\r\n";
		for (int i = 0; i< fnTable.m_FuncNum; i++){
			snprintf(msg, 1000, "%s\r\n", fnTable.GetFunction(i)->m_szName);			
			r += msg;
		}
		r += "===========================\r\n";
		return ret;

	}
public:
	CClassDes(cParser* p);
public:
	~CClassDes(void);
};

#endif //__CLASSDES_H__