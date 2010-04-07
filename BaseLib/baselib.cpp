// psstdlib.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "exp.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "baselib_version.h"

#ifdef _MACOS
#define __stdcall 
#endif
extern "C"{
void __stdcall put_str(char* msg)
{
	if (msg == NULL)
		return;
	printf(msg);
	printf("\n");
}


void __stdcall putl(long l)
{
	printf("%ld", l);
	return;
}

void __stdcall putf(float f)
{
	printf("%f", f);
	return;
}
void __stdcall put(char* format, long data)
{
	printf(format, data);
	return;
}
/*
// string manipulation
char* __stdcall strcpy(char* dest, char* src)
{
	if (dest == NULL || src == NULL)
		throw new CExp(CExp::err_invalid_param, "invalid param", CExp::stat_null);
	return strcpy(dest, src);
}*/


float __stdcall AToF(char* string)
{
	float ret = 0;
	if (string == NULL || strlen(string) == 0)
	{
		throw new CExp(CExp::err_invalid_param, "invalid param", CExp::stat_null);
		//return ret;
	}
	ret = (float)atof(string);
	return ret;
}

}

