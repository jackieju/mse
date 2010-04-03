#include "ObjTable.h"

CObjTable::CObjTable(void)
{
	hash_code = 0;
	this->m_lock.Create(NULL);
}

CObjTable::~CObjTable(void)
{
}
