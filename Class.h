#ifndef __CLASS_H__
#define __CLASS_H__
#include <string>
#include "ClassDes.h"

class CClass
{
private:
	std::string m_name;
public:
	char* GetFullName(){
		return (char*)m_name.c_str();
	}
public:
	CClass(CClassDes* cc);
public:
	~CClass(void);
};

#endif //__CLASS_H__