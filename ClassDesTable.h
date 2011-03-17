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
	struct str_hash{ size_t operator()(const std::string& str) const { return __stl_hash_string(str.c_str()); } };
private:
	hash_map<std::string, CClassDes*, str_hash> tables;
public: 
	void addClass(CClassDes* ccd){
		char* szName = ccd->GetFullName();
		if (getClass(szName) != NULL){
			char msg[1024] = "";
			snprintf(msg, 1000, "class overwritten", szName );
			ERR(msg);			
		}
		tables[szName] = ccd;
		printf("====>add class %s, %x\n", szName, ccd);
		printf("this=%x, %x\n", this, tables[szName]);
		tables["123"] = (CClassDes* )0x123456;
		char m[100]="";
		sprintf(m, "123");
		printf("this=%x, %x\n", this, tables[m]);
		printf(" tables[\"test/test\"]%x\n", tables["test/test"]);
	}

	CClassDes* getClass(char* szName){
			printf("getClass [\"test/test\"]%x\n", tables["test/test"]);
		return tables[szName];
	}
	void removeClass(char* szName);

	void dump(){
		printf("====== dump classdes table ======\n");
		printf("table size %d\n", tables.size());
		hash_map<std::string, CClassDes*, str_hash>::iterator it;
		
		//for ( it=tables.begin() ; it != tables.end(); it++ ){
			it=tables.begin() ;
			printf("%s==>%x\n", (*it).first,  (*it).second);
			printf("====dfaf\n");
			//it=tables.end();
			//printf("====dfaf\n");
			//printf("%d\n", it==tables.end());
			//printf("%s=>%x\n", (*it).first,  (*it).second);
			it++;
			printf("%s==>%x\n", (*it).first,  (*it).second);
				it++;
			printf("%s==>%x\n", (*it).first,  (*it).second);			it++;
			printf("%s==>%x\n", (*it).first,  (*it).second);			it++;
			printf("%s==>%x\n", (*it).first,  (*it).second);			it++;
			printf("%s==>%x\n", (*it).first,  (*it).second);			it++;
			printf("%s==>%x\n", (*it).first,  (*it).second);			it++;
			printf("%s==>%x\n", (*it).first,  (*it).second);			it++;
			printf("%s==>%x\n", (*it).first,  (*it).second);			it++;
			printf("%s==>%x\n", (*it).first,  (*it).second);			it++;
			printf("%s==>%x\n", (*it).first,  (*it).second);			it++;
			printf("%s==>%x\n", (*it).first,  (*it).second);
			it++;
			printf("%s==>%x\n", (*it).first,  (*it).second);
			it++;
			printf("%s==>%x\n", (*it).first,  (*it).second);
		//	printf("%s=>%x\n", (*it).first,  (*it).second);
		//}
			printf("this=%x, %x\n", this, tables["test/test"]);
		printf("==============================\n");
	}
	
public:
	CClassDesTable(void);
public:
	~CClassDesTable(void);
};

#endif //__CLASSTABLE_H__
