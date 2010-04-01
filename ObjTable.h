#ifndef __OBJTABLE_H__
#define __OBJTABLE_H__
#include "clib.h"
#include "os/CSS_LOCKEX.h"
#include "ObjectInst.h"

#ifdef WIN32
using namespace stdext;
#else
#include <ext/hash_map>
#endif


class CObjTable
{
private:
	std::vector<CObjectInst*> m_table;
	CSS_MUTEX m_lock;
	int hash_code;
public:
	CObjectInst* createObjectInstance(CClass* c){

		// lock table
		m_lock.Lock();

		// create instance
		CObjectInst *obj = new CObjectInst(m_table.size(), c);
		
		// add to table
		m_table.push_back(obj);		
		
		// relase table;
		m_lock.Unlock();
	
		return obj;
	
	};
public:
	CObjTable(void);
public:
	~CObjTable(void);
};

#endif //__OBJTABLE_H__