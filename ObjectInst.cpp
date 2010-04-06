#include "ObjectInst.h"

CObjectInst::CObjectInst(long id, CClass* cls)
{
	this->id = id;
	this->cls = cls;
	ref = 0;
	
}

CObjectInst::~CObjectInst(void)
{
}


//CObjectInst* CObjectInst::createObject(CClass* c){
//	long id;
//	
//	CObjectInst * p = new CObjectInst(id, c);
//	
//	return p;
//}