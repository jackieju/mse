#ifndef _COMMONDEF_H
#define _COMMONDEF_H


#ifndef WIN32
//#define HMODULE long
//#define LPSTR char *
//#define FARPROC long
#endif
#ifdef WIN32
#ifndef PATH_SEPARATOR_C
#define PATH_SEPARATOR_C '\\'
#endif
#else
#define PATH_SEPARATOR '/'
#endif

typedef struct _tagCOMMAND
{
	long  opcode;       //指令码
	long  address_mode; //寻址方式
	char opnum;        //操作数的数
	long  op[4];        //操作数表
	int  line;         //源代码中的位置
}COMMAND, *PCOMMAND;

typedef struct _tagPubFuncParam
{
	unsigned char* pData;
	int size;
	int reflvl;
	_tagPubFuncParam* pNext;
}PUBFUNCPARAM;

//log
#define REPORT_COMPILE_ERROR(s) ERR(s)
#define REPORT_MEM_ERROR(s) ERR(s)
#define REPORT_ERROR(s, l) nTRACE(s, l)
//#define LOG(s, lvl){\
//printf("LOG:%s", s);\
//}

//#define LOG(msg, l) if (g_pLogFunc) g_pLogFunc(msg, __FILE__, __LINE__, l); else printf("%s\n", msg);



#endif// _COMMONDEF_H
