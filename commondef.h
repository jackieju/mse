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
	long  opcode;       //ָ����
	long  address_mode; //Ѱַ��ʽ
	char opnum;        //����������
	long  op[4];        //��������
	int  line;         //Դ�����е�λ��
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
