#include "clib.h"
#include "os/CSS_LOCKEX.h"
#include "ClassDesTable.h"


CClassDesTable::CClassDesTable(void)
{
}

CClassDesTable::~CClassDesTable(void)
{
}


	void CClassDesTable::removeClass(char* szName){
		CClassDes* p = tables[szName];
			printf("remove classdes %s\n", p->GetFullName());
		if (tables[szName])
			delete tables[szName];
		tables[szName] = NULL;
	//	tables.erase(tables[szName]);
	}