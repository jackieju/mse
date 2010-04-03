#ifndef __OBJECTINST_H__
#define __OBJECTINST_H__
#include <string>
#include "Class.h"
#ifdef WIN32
using namespace stdext;
#else
#include <ext/hash_map>
#endif

class CObjectInst
{
public:
	void addRef(){
		ref ++;
	}

	int getRef(){
		return ref;
	}

	CObjectInst * getMemberAddress(char* name){
		return members[name];
	}
	//static CObjectInst* createObject(CClass* c);
private:
	long id;
	CClass* cls;
	int ref;
#ifdef _MACOS
	hash_map<char*, CObjectInst*> members;
#else
	stdext::hash_map<std::string, CObjectInst*> members;
#endif


public:
	CObjectInst(long id, CClass* cls);
public:
	~CObjectInst(void);
};

#endif //__OBJECTINST_H__
