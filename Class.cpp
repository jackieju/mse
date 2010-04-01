#include "memory.h"
#include "ClassDes.h"
#include "Class.h"


CClass::CClass(CClassDes* ccd)
{
	m_name = ccd->GetFullName();
}

CClass::~CClass(void)
{
}
