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
//#include <ext/hash_map>
#endif


class CClassTable {
	struct str_hash{ size_t operator()(const std::string& str) const { return __stl_hash_string(str.c_str()); } };
private:
#ifdef _MACOS
	typedef hash_map<std::string, CClass*, str_hash>::iterator HASH_MAP_IT;
	hash_map<char*, CClass*> m_table;
#else
	typedef stdext::hash_map<std::string , CClass*, str_hash >::iterator HASH_MAP_IT;
	stdext::hash_map<std::string , CClass* > m_table;
#endif
	
public:

	CClass* createClassInst(CClassDes* ccd){
		CClass * cc = new CClass(ccd);
		addClass(cc);
		return cc;
	}
	void addClass(CClass* c){
		//std::string name = c->GetFullName();
		m_table[c->GetFullName()] = c;
	}

	CClass* getClass(char* szName){
		return m_table[szName];
	}
	
	void dump(){
		printf("====== dump class table ======\n");
		HASH_MAP_IT it;
	//	for ( it=m_table.begin() ; it != m_table.end(); it++ ){
	//		printf("%s=>%x\n", (*it).first,  (*it).second);
	//	}
		printf("==============================\n");
	}
	
public:
	CClassTable(){};
	~CClassTable(){};
};
#endif //__CLASSTABLE_H__
