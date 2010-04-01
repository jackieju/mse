#ifndef __CLASSTABLE_H__
#define __CLASSTABLE_H__
#include <string>
#ifdef _MACOS
#include <ext/hash_map>
#else
#include <hash_map>
#endif
#include "log.h"
#include "Class.h"
#ifdef WIN32
using namespace stdext;
#else
#include <ext/hash_map>
#endif

class CClassTable {
private:
#ifdef _MACOS
	hash_map<std::string, CClass*> m_table;
#else
	stdext::hash_map<std::string , CClass* > m_table;
#endif
	
public:

	CClass* createClassInst(CClassDes* ccd){
		CClass * cc = new CClass(ccd);
		addClass(cc);
		return cc;
	}
	void addClass(CClass* c){
		m_table[c->GetFullName()] = c;
	}

	CClass* getClass(char* szName){
		return m_table[szName];
	}
	
public:
	CClassTable(){};
	~CClassTable(){};
};
#endif //__CLASSTABLE_H__
