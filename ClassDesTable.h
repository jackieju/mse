#ifndef __CLASSDESTABLE_H__
#define __CLASSDESTABLE_H__
#include <string>
#include "ClassDes.h"

#ifdef WIN32
#include <hash_map>
using namespace stdext;
#else
#include <ext/hash_map>
#endif
class CClassDesTable
{
private:
	hash_map<std::string, CClassDes*> tables;
public: 
	void addClass(CClassDes* ccd){
		char* szName = ccd->GetFullName();
		if (getClass(szName) != NULL){
			char msg[1024] = "";
			snprintf(msg, 1000, "class overwritten", szName );
			ERR(msg);			
		}
		tables[szName] = ccd;
	}

	CClassDes* getClass(char* szName){
		return tables[szName];
	}


	
public:
	CClassDesTable(void);
public:
	~CClassDesTable(void);
};

#endif //__CLASSTABLE_H__
