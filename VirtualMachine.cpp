

/************************************
  REVISION LOG ENTRY
  Revision By: Weihua Ju
  Revised on 2003-5-15 15:36:18
  Comments: 
  fix a bug in GetStatus(), should use pCurCall, not m_pCurCall
 ************************************/


/************************************
  REVISION LOG ENTRY
  Revision By: Weihua Ju
  Revised on 2003-5-6 11:26:02
  Comments: 
  m_ToStop should be reset in Reset() function, or the machine will not run after the first run.
 ************************************/


/************************************
  REVISION LOG ENTRY
  Revision By: Weihua Ju
  Revised on 2003-4-21 12:26:16
  Comments: 
  In Run(), add try catch for call of _endcallpub;
  In _endcallpub, add code to safely exit befor throw up exception
 ************************************/


/************************************
  REVISION LOG ENTRY
  Revision By: Weihua Ju
  Revised on 2003-3-27 16:30:00
  Comments: 
  modify the AttachParam()
  when check size, the size_t can be greater than function specified size,
  but can not be less.
 ************************************/


/************************************
  REVISION LOG ENTRY
  Revision By: Weihua Ju
  Revised on 2003-3-11 17:49:25
  Comments: 
  chang the return type of CVirtualMachine::LoadFunction() and CVirtualMachine::_LoadFunc
  and AddExternalSpace from BOOL to void,
  and change the return type of CVirtualMachine::AttachParam() from BOOL to long
 ************************************/


/************************************
  REVISION LOG ENTRY
  Revision By: Weihua Ju
  Revised on 2003-3-11 17:49:25
  Comments: 
  modify the AttachParam()
  when check size, the size_t must be equel to function specification
 ************************************/




/************************************
  REVISION LOG ENTRY
  Revision By: Weihua Ju
  Revised on 2003-3-5 19:17:35
  Comments: 
  //	1. modify conctructor of CVirtualMachine, create m_Stopped with
  //  initial status false. If initial status is true, the HandleRequest
  //  thread will hang up on the CVirtualMachine::Reset() call in case 
  //  it before run(). 
************************************/


#include "clib.h"
#include "os/CSS_LOCKEX.h"
#include <valarray>
#include <stack>
#include <string>
#include <iostream>
#include <fstream>
#include <malloc.h>
#include <time.h>
//#include "io.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "stdlib.h"
#include "Exp.h"
#include "Function.h"
#include "ScriptFuncTable.h"
#include "debug.h"
#include "VMException.h"
//#include "DOCommon.h"
#include "se_interface.h"
#include "ClassDes.h"
#include "ClassTable.h"
#include "ObjectInst.h"
#include "ObjTable.h"
#include "VirtualMachine.h"
#include "VMManager.h"
//#include "sym_engine.h"

long CVirtualMachine::m_lDbgCmdID = 0;
long CVirtualMachine::m_lDbgCmdP1 = 0;
long CVirtualMachine::m_lDbgCmdP2 = 0;
CSS_MUTEX CVirtualMachine::m_DgbCmdLock;
char CVirtualMachine::m_szDbgCmdP3[1024];
std::string CVirtualMachine::m_sDbgOutMsg;	// return msg of debug command execution
long CVirtualMachine::m_lDbgOutCode = 0;	// return code of debug command execution

#ifdef WIN32
/*
*	Use asm to call a function
*	_HGDispatchCall make call to pfn. pArgs is prealloced, which size
*  is (nSizeArgs + _SCRATCH_SIZE)
*
*/
extern "C"
{
	__declspec(naked) void __stdcall
		_HGDispatchCall(long* /*ret*/, void* /*pfn*/, void* /*pArgs*/, unsigned long /*nSizeArgs*/ )
	{
		_asm
		{
			pop     edx         // edx = return address
				//				pop		eax			// BE "return to" address, but not here actually(Instead, in (long*)pStack[0])
				pop     ebx         // edx = ret
				pop     eax         // eax = pfn
				pop     ecx         // ecx = pArgs
				add     ecx,[esp]   // ecx += nSizeArgs (=scratch area)
				mov     [ecx],edx   // scratch[0] = return address
				sub     ecx,[esp]   // ecx = pArgs (again)
				mov     esp,ecx     // esp = pArgs (usually already correct)
				call    eax         // call member function
				mov     [ebx], eax  //mov return value
				ret                 // esp[0] should = scratch[0] = return address
		}
	}
}
#else
extern "C"
{
	
	 static	__inline__ __attribute__ ((naked)) void __stdcall
		_HGDispatchCall(long* /*ret*/, void* /*pfn*/, void* /*pArgs*/, unsigned long /*nSizeArgs*/ )
		  {
			  __asm__ __volatile__(
			  "popl %edx\n\t"
			  "popl %ebx\n\t"
			  "popl %eax\n\t"
			  "popl %ecx\n\t"
			  "addl (%esp),%ecx\n\t"
			  "movl %edx, (%ecx)\n\t"
			  "subl (%esp),%ecx\n\t"
				"movl %ecx,%esp\n\t"			
				"call %eax\n\t"			
				"movl %eax,(%ebx)\n\t"
				"ret");
	}
}
#endif

BOOL __stdcall CallPubFunc(long lParamSize, void* pfn, void* pParam, long* pRet)
{
	
	// debug code
	//printf("CallPub: ParamBlockSize = %d, pData[0]: %x\n", lParamSize, *(long*)pParam);		

		void* p = alloca(lParamSize+16/*_SCRATCH_SIZE*/);
	memset(p, 0, lParamSize+16);	
	memcpy(p, (void*)pParam, lParamSize);


#ifdef WIN32
	_HGDispatchCall(pRet, (void*)pfn, p, lParamSize);
#else

		
	
			__asm__ __volatile__ (
			//"pushl	%0;"
			//"pushl	%1;"
			//"pushl	%2;"
			//"pushl	%3;"			
			"movl %3, %%ebx;"	// edx = pRet
			"movl %2, %%eax;"	// eax = pfn
			"movl %1, %%ecx;"	// ecx = p
			"movl %0, %%edx;"	// edx = lParamSize
	//		"addl %%edx, %%ecx;"	// ecx = p + lParamsize = Scrach erea
	//		"movl %%edx,(%%ecx);"	// scrach = return address;
	//		"subl %%edx, %%ecx;"	// ecx = ecx - lParasize = p;
			"movl %%ecx,%%esp;"		// move stock point to p
			"call %%eax;"			// call function
			"movl %%eax,(%%ebx);"	: 	:"r"(lParamSize) , "r"(p) , "r"(pfn) , "r"(pRet)
			);	
	
#endif

	return TRUE;
}
#include "opcode.h"

//////////////////////////////////////////////////////////////////////
// Consuction/Destruction
//////////////////////////////////////////////////////////////////////

CVirtualMachine::CVirtualMachine()
{
//	m_BreakPoint = NULL;
//	m_BpList.size() = 0;
	__AX = __BX = __CX = __DX = __IP = __PSW = 0;
//	IsRunning = FALSE;
	m_pCurCall = NULL;
	m_lExternalSpaceListSize = 0;
	m_lExternalSpaceNum = 0;

//	m_pInputFn = NULL;
//	m_debugOut = stdout;
	m_nWorkMode = VM_MODE_NORMAL;
	m_pExternalSpaceList = NULL;
	
	m_bIsDebugBreak = FALSE;
	if (m_ToStop.Create(NULL, false, false, NULL) != LOCKEX_ERR_OK)
		throw new CVMMException("create stop event failed");
	// 20030305 modify by Weihua Ju
	// original code:
	//if (m_Stopped.Create(NULL, true, false, NULL) != LOCKEX_ERR_OK)
	//	throw new CVMMException("create stop event failed");
	// new code
	if (m_Stopped.Create(NULL, true, true, NULL) != LOCKEX_ERR_OK)
		throw new CVMMException("create stop event failed");
	// modification end

	if (m_StatusLock.Create(NULL) != LOCKEX_ERR_OK)
		throw new CVMMException("create stop event failed");
//	Reset();
}

CVirtualMachine::~CVirtualMachine()
{
	Reset();
}

void CVirtualMachine::Reset()
{
//	if (m_BreakPoint)
//		delete m_BreakPoint;
//	m_BreakPoint = NULL;
//	m_BpList.size() = 0;
//	IsRunning = FALSE;
	m_ToStop.Set();		// stop the machine
	m_Stopped.Wait();	// wait machine stop
	m_ToStop.Reset();	


	__AX = __BX = __CX = __DX = __IP = __PSW = 0;
	/*	if (m_PointSeg)
	{
	delete m_PointSeg;
	m_PointSeg = NULL;
	}
	*/

	m_StatusLock.Lock();
	
	while (m_CallStack.empty() == false)		
	{
		m_pCurCall = &(m_CallStack.back());
		if (m_pCurCall->VMemory)
		{
			delete m_pCurCall->VMemory;			
		}
		m_CallStack.pop_back();		
	}	
	memset(&m_StartTime, 0, sizeof(SYSTEMTIME));
	m_StatusLock.Unlock();

	m_pCurCall = NULL;
	FUNCTIONSTACKELE *pCallInfo;
	while( m_FuncStack.empty() == false)
	{
		pCallInfo = m_FuncStack.top();
		PUBFUNCPARAM* param = &pCallInfo->paramhdr;
		PUBFUNCPARAM* paramsparent = NULL;
		while (pCallInfo->paramPt != &pCallInfo->paramhdr)
		{
			while (param != pCallInfo->paramPt)
			{
				//get last param
				paramsparent = param;
				param = param->pNext;
			}
			delete param;
			//go up;
			pCallInfo->paramPt = paramsparent;
			param = &pCallInfo->paramhdr;
		}
//		pCallInfo->paramPt = &pCallInfo->paramhdr;
		delete pCallInfo;
		m_FuncStack.pop();
	}
	if (m_pExternalSpaceList)
	{
		delete m_pExternalSpaceList;
		m_pExternalSpaceList = NULL;
	}
	m_lExternalSpaceListSize = 0;
	m_lExternalSpaceNum = 0;

//	m_pInputFn = NULL;
//	m_debugOut = stdout;
	m_nWorkMode = VM_MODE_NORMAL;
}


/*
����������	BOOL CVirtualMachine::LoadFunction(CFunction *pFunc)
�������ܣ�	���ؽű�����
����˵����	
			[IN]CFunction *pFunc	-	����ָ��
�� �� ֵ��	BOOL  - �ɹ���ʧ��
�� д �ˣ�	������
������ڣ�	2001-6-26
*/
void CVirtualMachine::LoadFunction(CFunction *pFunc)
{
//	BOOL bRet;
	m_StatusLock.Lock();
//	bRet = _LoadFunc(pFunc);
	_LoadFunc(pFunc);
	m_StatusLock.Unlock();

//	return bRet;
}

void CVirtualMachine::_LoadFunc(CFunction *pFunc)
{
	if (pFunc == NULL)
		throw new CVMMException("input CFuntion is null");	
	
	//	m_nWorkMode = pFunc->m_nWorkMode;
	if (m_pCurCall != NULL) // �����ֳ�
	{
		m_pCurCall->IP = __IP;
	}
	

	//���������ڴ�
	int TotalSize = pFunc->m_SymbolTable.m_nTotalSize + pFunc->m_nSSUsedSize;
	CALLSTACKELE newStackEle;
	memset(&newStackEle, sizeof(CALLSTACKELE), 0);
	CALLSTACKELE* pStackEle = &newStackEle;
	if (pStackEle == NULL)
	{
	//	REPORT_MEM_ERROR("not enough memory, when virtual machine create CALLSTACK ELEMENT\r\n");
		throw new CVMMException("not enough memory, when virtual machine create CALLSTACK ELEMENT");
	//	return FALSE;
	}
	pStackEle->pFunc = pFunc;
	pStackEle->IP = 0;
	pStackEle->VMemory = new unsigned char[TotalSize];
	if (pStackEle->VMemory == NULL)
	{
		//REPORT_MEM_ERROR("not enough memory, when virtual machine create virtual memory\r\n")
		throw new CVMMException("not enough memory, when virtual machine create virtual memory");	
	//	return FALSE;
	}
	//	clear virtual memory
	memset(pStackEle->VMemory, 0, TotalSize);
	pStackEle->ulVMemSize = TotalSize;
	pStackEle->ulDataSegSize = pFunc->m_SymbolTable.m_nTotalSize;
	pStackEle->ulSSize = pFunc->m_nSSUsedSize;	
	pStackEle->DataSeg = pStackEle->VMemory + pFunc->m_nSSUsedSize;
	pStackEle->StaticSeg = pStackEle->VMemory;

	// push to call stack
	m_CallStack.push_back(*pStackEle);
	m_pCurCall = &(m_CallStack.back());

	if (pFunc->m_staticSegment)
		memcpy(pStackEle->StaticSeg, pFunc->m_staticSegment, pFunc->m_nSSUsedSize);

	__IP = 0;
//	return TRUE;
}

#define CMD_PREPROCESS2 \
	if (cmd == NULL)\
{\
	nLOG("SE:: commond is NUll", 50);\
	return FALSE;\
}\
	int op1mode, op2mode, op2reflvl, op1reflvl;\
	unsigned char *src, *dest;\
	if (!Preprocess2(cmd, op1mode, op1reflvl, op2mode, op2reflvl, dest, src))\
return FALSE;

#define CMD_PREPROCESS1 \
	if (cmd == NULL)\
{\
	nLOG("SE:: commond is NUll", 50);\
	return FALSE;\
}\
	int op1mode, op1reflvl;\
	unsigned char * dest;\
if (!Preprocess1(cmd, op1mode, op1reflvl, dest)) return FALSE;



BOOL CVirtualMachine::_mov(PCOMMAND cmd)
{
	CMD_PREPROCESS2	
		short opsize;//���ֽڲ���, �ֲ���, ��˫�ֲ���
//	opsize = __min((cmd->address_mode >> 14)&0x3 ,(cmd->address_mode >> 6)&0x3);
    if (((cmd->address_mode >> 14)&0x3)>((cmd->address_mode >> 6)&0x3))
        opsize = (cmd->address_mode >> 6)&0x3;
    else
        opsize = (cmd->address_mode >> 14)&0x3;
	switch (opsize)
	{
	case 0:	memcpy(dest, src, 1);break;
	case 1:	memcpy(dest, src, 2);break;
	case 2:	memcpy(dest, src, 4);break;
	case 3:	memcpy(dest, src, 8);break;
	}
	__IP++;
	return TRUE;
}

BOOL CVirtualMachine::_add(PCOMMAND cmd)
{	
	CMD_PREPROCESS2
		short opsize;//���ֽڲ���, �ֲ���, ��˫�ֲ���
	//opsize= __max( (cmd->address_mode >> 14)&0x3, (cmd->address_mode >> 6)&0x3);
	if ( ((cmd->address_mode >> 14)&0x3)>((cmd->address_mode >> 6)&0x3))
	    opsize = (cmd->address_mode >> 14)&0x3;
    else
        opsize = (cmd->address_mode >> 6)&0x3;
	switch (opsize)
	{
	case 0:	__AX = *((char*)dest) + *((char*)src);break;
	case 1:	__AX = *((short*)dest) + *((short*)src);break;
	case 2:	__AX = *((int*)dest) + *((int*)src);break;
	case 3:	__AX = *((long*)dest) + *((long*)src);break;
	}
	__IP++;
	return TRUE;
}

BOOL CVirtualMachine::_mul(PCOMMAND cmd)
{
	CMD_PREPROCESS2
		short opsize;//���ֽڲ���, �ֲ���, ��˫�ֲ���
	//opsize= __max( (cmd->address_mode >> 14)&0x3, (cmd->address_mode >> 6)&0x3);
	if ( ((cmd->address_mode >> 14)&0x3)>((cmd->address_mode >> 6)&0x3))
	    opsize = (cmd->address_mode >> 14)&0x3;
    else
        opsize = (cmd->address_mode >> 6)&0x3;
	switch (opsize)
	{
	case 0:	__AX = *((char*)dest) * *((char*)src);break;
	case 1:	__AX = *((short*)dest) * *((short*)src);break;
	case 2:	__AX = *((int*)dest) * *((int*)src);break;
	case 3:	__AX = *((long*)dest) * *((long*)src);break;
	}
	
	__IP++;
	return TRUE;
}



BOOL CVirtualMachine::_not(PCOMMAND cmd)
{
	CMD_PREPROCESS1
		short opsize = (cmd->address_mode >> 14)&0x3;
	switch (opsize)
	{
	case 0: *(char*)dest = ~(*(char*)dest);break;
	case 1: *(short*)dest = ~(*(short*)dest);break;
	case 2: *(int*)dest = ~(*(int*)dest);break;
	case 3: *(long*)dest = ~(*(long*)dest);break;
		
	}
	/*	int temp = ~(*((int*)dest));
	memcpy(dest, &temp, (cmd->address_mode >> 14)&0x3);
	*/	__IP++;
	return TRUE;
}

BOOL CVirtualMachine::_notr(PCOMMAND cmd)
{
	CMD_PREPROCESS1
		short opsize = (cmd->address_mode >> 14)&0x3;
	switch (opsize)
	{
	case 0: 
		*(char*)dest = !(*(char*)dest);
/*		if (*(char*)dest)
			__PSW |= 0x1;
		else
			__PSW &= 0xfffe;
*/		break;
	case 1: 
		*(short*)dest = !(*(short*)dest);
/*		if (*(short*)dest)
			__PSW |= 0x1;
		else
			__PSW &= 0xfffe;
*/		break;
	case 2: 
		*(int*)dest = !(*(int*)dest);
/*		if (*(int*)dest)
			__PSW |= 0x1;
		else
			__PSW &= 0xfffe;
*/		break;
	case 3:
		*(long*)dest = !(*(long*)dest);
/*		if (*(long*)dest)
			__PSW |= 0x1;
		else
			__PSW &= 0xfffe;
*/		break;
	}

	/*	int temp = ~(*((int*)dest));
	memcpy(dest, &temp, (cmd->address_mode >> 14)&0x3);
	*/	__IP++;
	return TRUE;
}

BOOL CVirtualMachine::_sub(PCOMMAND cmd)
{
	CMD_PREPROCESS2
		short opsize;//���ֽڲ���, �ֲ���, ��˫�ֲ���
//	opsize= __max( (cmd->address_mode >> 14)&0x3, (cmd->address_mode >> 6)&0x3);
    if (( (cmd->address_mode >> 14)&0x3)>((cmd->address_mode >> 6)&0x3))
        opsize = (cmd->address_mode >> 14)&0x3;
    else
        opsize = (cmd->address_mode >> 6)&0x3;
	switch (opsize)
	{
	case 0:	__AX = *((char*)dest) - *((char*)src);break;
	case 1:	__AX = *((short*)dest) - *((short*)src);break;
	case 2:	__AX = *((int*)dest) - *((int*)src);break;
	case 3:	__AX = *((long*)dest) - *((long*)src);break;
	}
	__IP++;
	
	return TRUE;
}

BOOL CVirtualMachine::_div(PCOMMAND cmd)
{
	char szMsg[201] = "";
	CMD_PREPROCESS2
		short opsize;//���ֽڲ���, �ֲ���, ��˫�ֲ���
	//opsize= __max( (cmd->address_mode >> 14)&0x3, (cmd->address_mode >> 6)&0x3);
    if (( (cmd->address_mode >> 14)&0x3)>((cmd->address_mode >> 6)&0x3))
        opsize = (cmd->address_mode >> 14)&0x3;
    else
        opsize = (cmd->address_mode >> 6)&0x3;
	switch (opsize)
	{
	case 0:	
		if (*((char*)src) ==0)
		{
			snprintf(szMsg, 200, "VM:: devide zero error, function='%s', line=%d, IP=%d\r\n", m_pCurCall->pFunc->m_szName, cmd->line, __IP);
			REPORT_ERROR(szMsg, 20);
			return FALSE;
		}
		__AX = *((char*)dest) / *((char*)src);
		break;
	case 1:	
		if (*((short*)src) ==0)
		{
			snprintf(szMsg, 200, "VM:: devide zero error, function='%s', line=%d, IP=%d\r\n", m_pCurCall->pFunc->m_szName, cmd->line, __IP);
			REPORT_ERROR(szMsg, 20);
			return FALSE;
		}
		__AX = *((short*)dest) / *((short*)src);
		break;
	case 2:	
		if (*((int*)src) ==0)
		{
			snprintf(szMsg, 200, "VM:: devide zero error, function='%s', line=%d, IP=%d\r\n", m_pCurCall->pFunc->m_szName, cmd->line, __IP);
			REPORT_ERROR(szMsg, 20);
			return FALSE;
		}
		__AX = *((int*)dest) / *((int*)src);
		break;
	case 3:	
		if (*((long*)src) ==0)
		{
			snprintf(szMsg, 200, "VM:: devide zero error, function='%s', line=%d, IP=%d\r\n", m_pCurCall->pFunc->m_szName, cmd->line, __IP);
			REPORT_ERROR(szMsg, 20);
			return FALSE;
		}
		__AX = *((long*)dest) / *((long*)src);
		break;
	}
	
	__IP++;
	
	return TRUE;
}

BOOL CVirtualMachine::_mod(PCOMMAND cmd)
{
	char szMsg[201] = "";
	CMD_PREPROCESS2
		short opsize;//���ֽڲ���, �ֲ���, ��˫�ֲ���
	//  opsize= __max( (cmd->address_mode >> 14)&0x3, (cmd->address_mode >> 6)&0x3);
    if (( (cmd->address_mode >> 14)&0x3)>((cmd->address_mode >> 6)&0x3))
        opsize = (cmd->address_mode >> 14)&0x3;
    else
        opsize = (cmd->address_mode >> 6)&0x3;
	switch (opsize)
	{
	case 0:	
		if (*((char*)src) ==0)
		{
			snprintf(szMsg, 200, "VM:: devide zero error, function='%s', line=%d, IP=%d\r\n", m_pCurCall->pFunc->m_szName, cmd->line, __IP);
			REPORT_ERROR(szMsg, 20);
			return FALSE;
		}
		__AX = *((char*)dest) % *((char*)src);
		break;
	case 1:	
		if (*((short*)src) ==0)
		{
			snprintf(szMsg, 200, "VM:: devide zero error, function='%s', line=%d, IP=%d\r\n", m_pCurCall->pFunc->m_szName, cmd->line, __IP);
			REPORT_ERROR(szMsg, 20);
			return FALSE;
		}
		__AX = *((short*)dest) % *((short*)src);
		break;
	case 2:	
		if (*((int*)src) ==0)
		{
			snprintf(szMsg, 200, "VM:: devide zero error, function='%s', line=%d, IP=%d\r\n", m_pCurCall->pFunc->m_szName, cmd->line, __IP);
			REPORT_ERROR(szMsg, 20);
			return FALSE;
		}
		__AX = *((int*)dest) % *((int*)src);
		break;
	case 3:	
		if (*((long*)src) ==0)
		{
			snprintf(szMsg, 200, "VM:: devide zero error, function='%s', line=%d, IP=%d\r\n", m_pCurCall->pFunc->m_szName, cmd->line, __IP);
			REPORT_ERROR(szMsg, 20);
			return FALSE;
		}
		__AX = *((long*)dest) % *((long*)src);
		break;
	}
	
	__IP++;
	
	return TRUE;
}

/*
��������     : CVirtualMachine::_jz
��������	    : ���__PSW�ĵ���λ��1, ����ת
����˵��     : 
����ֵ       : 
��д��       : ������
�������     : 2001 - 5 - 11
*/
BOOL CVirtualMachine::_jz(PCOMMAND cmd)
{
	CMD_PREPROCESS1
		if (__PSW&0x1 != 0)
			__IP = *((int*)dest);
		else
			__IP++;
		return TRUE;
}


/*
��������     : CVirtualMachine::_jnz
��������	    : ���__PSW�ĵ���λ��0, ����ת
����˵��     : 
����ֵ       : 
��д��       : ������
�������     : 2001 - 5 - 11
*/
BOOL CVirtualMachine::_jnz(PCOMMAND cmd)
{
	CMD_PREPROCESS1
		if ((__PSW&0x1) == 0)
			__IP = *((int*)dest);
		else
			__IP++;
		return TRUE;
}

BOOL CVirtualMachine::_jmpz(PCOMMAND cmd)
{
	CMD_PREPROCESS2
		if (*((int*)dest) != 0)
			__IP = *((int*)src);
		else
			__IP++;
		return TRUE;
}

BOOL CVirtualMachine::_jmp(PCOMMAND cmd)
{
	CMD_PREPROCESS1
		__IP = *((int*)dest);
	return TRUE;
}

BOOL CVirtualMachine::_ea(PCOMMAND cmd)
{
	int EA;
	CMD_PREPROCESS2
		EA = (long)src;
	memcpy(dest, &EA, sizeof(long));
	__IP++;
	return TRUE;
}

BOOL CVirtualMachine::_eaobj(PCOMMAND cmd)
{
	int EA;
 
	unsigned char* dest = (unsigned char*)&(m_pCurCall->DataSeg[cmd->op[0]]);
	unsigned char* obj = &m_pCurCall->DataSeg[cmd->op[1]];

	if (cmd->opnum > 2){
		if (obj == NULL)
			return FALSE;
		char* member = (char*)(&(m_pCurCall->StaticSeg[cmd->op[2]]));
		EA = (int)(((CObjectInst*)obj)->getMemberAddress(member));
	}else
		EA = (long)obj;
	memcpy(dest, &EA, sizeof(long));
	__IP++;
	return TRUE;
}
/*

int exception_filter(EXCEPTION_POINTERS * pex)
{
	std::ofstream of("exception_report.txt", ios_base::out | ios_base::app);
	SYSTEMTIME time;
	GetLocalTime(&time);
	of << std::endl << "---------------------------------------------------";
	of << std::endl <<"Exception occured on " << time.wYear << time.wMonth << time.wDay << time.wHour << time.wMinute << time.wSecond << time.wMilliseconds << std::endl;
	sym_engine::stack_trace(of, pex->ContextRecord);		
	of.flush();
	return EXCEPTION_EXECUTE_HANDLER ;
}
*/
BOOL CVirtualMachine::_endcallpub(PCOMMAND cmd)
{
	// get current function call info
	FUNCTIONSTACKELE *pCallInfo = NULL;
	if (m_FuncStack.empty() == true)
		return FALSE;
	pCallInfo = m_FuncStack.top();/* line = %x,*/
//	printf("&pCallInfo =%x, IP = (%x)", &pCallInfo,  /*cmd->line,*/ &__IP);

	//generate parametre block
	PUBFUNCPARAM* param, *paramsparent;
	param = &pCallInfo->paramhdr;

	void* pData = (void*) new char[pCallInfo->lParamBlockSize];
	memset(pData, 0, pCallInfo->lParamBlockSize);
	unsigned char* pDataPt = (unsigned char*)pData;
	while (param != pCallInfo->paramPt)
	{
		if ((BYTE*)pDataPt + param->size - (BYTE*)pData > pCallInfo->lParamBlockSize)
		{
			
			pCallInfo->lParamBlockSize = 0;
			delete pData;
			param = &pCallInfo->paramhdr;
			
			while (pCallInfo->paramPt != &pCallInfo->paramhdr)
			{
				while (param != pCallInfo->paramPt)
				{
					//get last param
					paramsparent = param;
					param = param->pNext;
				}
				delete param;
				//go up;
				pCallInfo->paramPt = paramsparent;
				param = &pCallInfo->paramhdr;
			}
			pCallInfo->paramPt = &pCallInfo->paramhdr;
			return FALSE;
		}
		memcpy(pDataPt, param->pData, param->size);
		pDataPt += param->size;		
		if (param->size < 4)
		{
			pDataPt += 4- param->size;
		}
		/*		//ֻ����һ��ָ��, ��������ָ���ָ��
		if (param->reflvl > 0)
		*((int*)(param->pData)) += (int)memory;
		*/
		param = param->pNext;
	}
#if 0		
	//call function
	{
		void* p = alloca(pCallInfo->lParamBlockSize+16/*_SCRATCH_SIZE*/);
		memset(p, 0, pCallInfo->lParamBlockSize+16);	
		memcpy(p, (void*)pData, pCallInfo->lParamBlockSize);
//	printf("2:param = %x, param->size = %x, param->next = %x, &pCallInfo =%x\n", param, param->size, param->pNext, &pCallInfo);
		_HGDispatchCall(&__AX, (void*)pCallInfo->fn, p, pCallInfo->lParamBlockSize);		
//	printf("2:param = %x, param->size = %x, param->next = %x, &pCallInfo =%x\n", param, param->size, param->pNext, &pCallInfo);
	}
#endif

	BOOL ret = TRUE;
	
	char msg[201] = "";

/*
	try
	{
		CallPubFunc(pCallInfo->lParamBlockSize, (void*)pCallInfo->fn, (void*)pData, &__AX);		
	}
	catch(CVMException *e)
	{
		long lCode;
		char* sMsg = NULL;
		sMsg = e->GetExp(lCode);
		snprintf(msg, 200, "SE:: fatal error - external function call cause VM exception. script: %s, line: %d, Exception Error(%d): %s", m_pCurCall->pFunc->m_szName, cmd->line, lCode, sMsg);
		LOG(sMsg, 5);
		ret = FALSE;
		delete e;
	}

 */

/*	cactch all exception, project must not use the /GX link option, and "CallPubFunc" must be a __stdcall function
*/

	try
	{
		CallPubFunc(pCallInfo->lParamBlockSize, (void*)pCallInfo->fn, (void*)pData, &__AX);		
	}
	catch(CVMException *e)
	{
		long lCode;
		char* sMsg = NULL;
		sMsg = e->GetExp(lCode);
		snprintf(msg, 200, "SE:: fatal error - external function call cause VM exception. script: %s, line: %d, Exception Error(%d): %s", m_pCurCall->pFunc->m_szName, cmd->line, lCode, sMsg);
		nLOG(msg, 0);		
		ret = FALSE;
		delete e;

		// dump memroy to file
		DumpToFile();
	}
	catch (CExp* e)
	{
		snprintf(msg, 200, "SE::  Got exception out of virtual machine. script: %s, line: %d", m_pCurCall->pFunc->m_szName, cmd->line);
		nLOG(msg, 0);
		ret = FALSE;
		
		// dump memroy to file
		//DumpToFile();
		
		// before throw, shuld exit safely
		//clear
		pCallInfo->lParamBlockSize = 0;
		delete pData;
		param = &pCallInfo->paramhdr;
		
		while (pCallInfo->paramPt != &pCallInfo->paramhdr)
		{
			while (param != pCallInfo->paramPt)
			{
				//get last param
				paramsparent = param;
				param = param->pNext;
			}
			delete param;
			paramsparent->pNext = NULL;
			//go up;
			pCallInfo->paramPt = paramsparent;
			param = &pCallInfo->paramhdr;
		}
		delete pCallInfo;
		m_FuncStack.pop();
		
		__IP++;
		//	printf("&pCallInfo =%x, line = %x, IP = %x(%x)", &pCallInfo, cmd->line, __IP, &__IP);
		
		//	return ret;		// throw to system and let system dump memory
		CExp * exp = new CExp(0, msg, __FILE__, __LINE__);
		exp->SetLow(e);
		throw exp;
	}
	catch (...)
	{
		snprintf(msg, 200, "SE:: fatal error - external function call cause unknown exception. script: %s, line: %d", m_pCurCall->pFunc->m_szName, cmd->line);
		nLOG(msg, 0);
		ret = FALSE;
		
		// dump memroy to file
		DumpToFile();

		// before throw, shuld exit safely
		//clear
		pCallInfo->lParamBlockSize = 0;
		delete pData;
		param = &pCallInfo->paramhdr;
		
		while (pCallInfo->paramPt != &pCallInfo->paramhdr)
		{
			while (param != pCallInfo->paramPt)
			{
				//get last param
				paramsparent = param;
				param = param->pNext;
			}
			delete param;
			paramsparent->pNext = NULL;
			//go up;
			pCallInfo->paramPt = paramsparent;
			param = &pCallInfo->paramhdr;
		}
		delete pCallInfo;
		m_FuncStack.pop();
		
		__IP++;
		//	printf("&pCallInfo =%x, line = %x, IP = %x(%x)", &pCallInfo, cmd->line, __IP, &__IP);
		
	//	return ret;		// throw to system and let system dump memory
		throw;
	}
		
/* use dbghelp api to get exception position, must not use the /O2 compilation option
	__try
	{
			CallPubFunc(pCallInfo->lParamBlockSize, (void*)pCallInfo->fn, (void*)pData, &__AX);		
	}
	__except (exception_filter(GetExceptionInformation()))
	{
		snprintf(msg, 200, "SE:: fatal error - external function call cause unknown exception. script: %s, line: %d", m_pCurCall->pFunc->m_szName, cmd->line);
		nLOG((msg, 5);
		ret = FALSE;
	}
*/
	//clear
	pCallInfo->lParamBlockSize = 0;
	delete pData;
	param = &pCallInfo->paramhdr;
	
//	printf("3:param = %x, param->size = %x, param->next = %x\n", param, param->size, param->pNext);
	while (pCallInfo->paramPt != &pCallInfo->paramhdr)
	{
		while (param != pCallInfo->paramPt)
		{
			//get last param
			paramsparent = param;
			param = param->pNext;
		}
		delete param;
		paramsparent->pNext = NULL;
		//go up;
		pCallInfo->paramPt = paramsparent;
		param = &pCallInfo->paramhdr;
	}
	//pCallInfo->paramPt = &pCallInfo->paramhdr;
	delete pCallInfo;
	m_FuncStack.pop();

	__IP++;
//	printf("&pCallInfo =%x, line = %x, IP = %x(%x)", &pCallInfo, cmd->line, __IP, &__IP);

	return ret;
}

BOOL CVirtualMachine::_callpub(PCOMMAND cmd)
{
	if (cmd == NULL)
	{
		nLOG("SE:: commond is NUll", 50);
		return FALSE;
	}
	int op1mode, op1reflvl;
	unsigned char * dest;
	if (!Preprocess1(cmd, op1mode, op1reflvl, dest, FALSE)) return FALSE;
	
	FUNCTIONSTACKELE *newCall = new FUNCTIONSTACKELE;
	if (newCall == NULL)
	{
		nLOG("SE:: allocate memory failed", 5);
		return FALSE;
	}
	memset(newCall, 0, sizeof(FUNCTIONSTACKELE));
	newCall->paramPt = &newCall->paramhdr;
	newCall->fn = *((long*)dest);
	m_FuncStack.push(newCall);

	__IP++;
	return TRUE;
}

BOOL CVirtualMachine::_parampub(PCOMMAND cmd)
{
	CMD_PREPROCESS1
	short opsize;// ���ֽڲ���, �ֲ���, ��˫�ֲ���
	
	// get opsize
	opsize= (cmd->address_mode >> 14)&0x3;
	int size = 1;
	for (int i = 0; i< opsize; i++)
	{
		size *= 2;
	}	

	// get current function call info
	FUNCTIONSTACKELE *pCallInfo = NULL;
	if (m_FuncStack.empty() == true)
		return FALSE;
	pCallInfo = m_FuncStack.top();

	pCallInfo->paramPt->pData = (unsigned char*)dest;
	pCallInfo->paramPt->size = size;
	pCallInfo->paramPt->reflvl = op1reflvl;
	pCallInfo->lParamBlockSize += pCallInfo->paramPt->size;
	// �ֽڶ���
	if (pCallInfo->paramPt->size < 4)
	{
		pCallInfo->lParamBlockSize+= 4-pCallInfo->paramPt->size;
	}

	pCallInfo->paramPt->pNext = new PUBFUNCPARAM;
	pCallInfo->paramPt = pCallInfo->paramPt->pNext;
	pCallInfo->paramPt->pData = NULL;
	pCallInfo->paramPt->pNext = NULL;
	pCallInfo->paramPt->size = 0;
	
	// debug code
	//printf("param: pData[0] = %x, size = %d, reflvl = %d\n", *(unsigned long*)dest, size, op1reflvl);
//	printf("param: address_mode = %x, opcode\n", cmd->address_mode, cmd->opcode[0]);
	__IP++;
	return TRUE;
}

BOOL CVirtualMachine::_callv(PCOMMAND cmd)
{
	if (cmd == NULL)
	{
		nLOG("SE:: commond is NUll", 50);
		return FALSE;	
	}
	

	int op1mode, op1reflvl;
	unsigned char * dest;
	if (!Preprocess1(cmd, op1mode, op1reflvl, dest, FALSE)) return FALSE;



	FUNCTIONSTACKELE *newCall = new FUNCTIONSTACKELE;
	if (newCall == NULL)
	{
		nLOG("SE:: allocate memory failed", 5);
		return FALSE;
	}

	
	memset(newCall, 0, sizeof(FUNCTIONSTACKELE));
	newCall->paramPt = &newCall->paramhdr;
	newCall->fn = *((long*)dest);
	m_FuncStack.push(newCall);

	__IP++;

	
	return TRUE;
}
BOOL CVirtualMachine::_paramv(PCOMMAND cmd)
{
	CMD_PREPROCESS1
	short opsize;// ���ֽڲ���, �ֲ���, ��˫�ֲ���
	
	// get opsize
	opsize= (cmd->address_mode >> 14)&0x3;
	int size = 1;
	for (int i = 0; i< opsize; i++)
	{
		size *= 2;
	}	
	FUNCTIONSTACKELE *pCallInfo = NULL;
	if (m_FuncStack.empty() == true)
		return FALSE;
	pCallInfo = m_FuncStack.top();



	pCallInfo->paramPt->pData = (unsigned char*)dest;
	pCallInfo->paramPt->size = size;
	pCallInfo->paramPt->reflvl = op1reflvl;
	pCallInfo->lParamBlockSize += pCallInfo->paramPt->size;

/*	if (pCallInfo->paramPt->size < 4)
	{
		pCallInfo->paramhdr+= 4-pCallInfo->paramPt->size;
	}
*/
	pCallInfo->paramPt->pNext = new PUBFUNCPARAM;
	pCallInfo->paramPt = pCallInfo->paramPt->pNext;
	pCallInfo->paramPt->pData = NULL;
	pCallInfo->paramPt->pNext = NULL;
	pCallInfo->paramPt->size = 0;

	__IP++;
	return TRUE;
}
BOOL CVirtualMachine::_endcallv(PCOMMAND cmd)
{
	//generate parametre block
	PUBFUNCPARAM* param, *paramsparent;
	
	// get current function call info
	FUNCTIONSTACKELE *pCallInfo = NULL;
	if (m_FuncStack.empty() == true)
		return FALSE;
	pCallInfo = m_FuncStack.top();
	param = &pCallInfo->paramhdr;

	void* pData = (void*) new char[pCallInfo->lParamBlockSize];
	if (pData == NULL)
	{
		nLOG("SE:: allocate memory failed", 5);
		return FALSE;
	}
	memset(pData, 0, pCallInfo->lParamBlockSize);
	unsigned char* pDataPt = (unsigned char*)pData;
	while (param != pCallInfo->paramPt)
	{
		if ((BYTE*)pDataPt + param->size - (BYTE*)pData > pCallInfo->lParamBlockSize)
		{// has error			
			delete pData;
			param = &pCallInfo->paramhdr;
			
			while (pCallInfo->paramPt != &pCallInfo->paramhdr)
			{
				while (param != pCallInfo->paramPt)
				{
					//get last param
					paramsparent = param;
					param = param->pNext;
				}
				delete param;
				//go up;
				pCallInfo->paramPt = paramsparent;
				param = &pCallInfo->paramhdr;
			}
			pCallInfo->paramPt = &pCallInfo->paramhdr;
			return FALSE;			
		}
		// copy data
		memcpy(pDataPt, param->pData, param->size);
		pDataPt += param->size;		
		param = param->pNext;
	}
	
	// load function
	__IP++;
	_LoadFunc((CFunction*)pCallInfo->fn);
	AttachParam((BYTE*)pData, pCallInfo->lParamBlockSize);
	
	//clear
	if (pData) delete pData;
	param = &pCallInfo->paramhdr;
	
	// release link table of param info
	while (pCallInfo->paramPt != &pCallInfo->paramhdr)
	{
		while (param != pCallInfo->paramPt)
		{
		//	printf("param = %x, param->size = %x\n", param, param->size);
			//get last param
			paramsparent = param;
			param = param->pNext;
		}
		delete param;
		paramsparent->pNext = NULL;
		//go up;
		pCallInfo->paramPt = paramsparent;
		param = &pCallInfo->paramhdr;
	}
	//pCallInfo->paramPt = &pCallInfo->paramhdr;
	delete pCallInfo;
	m_FuncStack.pop();


	__IP = 0;
	return TRUE;
}


BOOL CVirtualMachine::_test(PCOMMAND cmd)
{
	CMD_PREPROCESS2
		short opsize;//���ֽڲ���, �ֲ���, ��˫�ֲ���
	//	opsize= __max( (cmd->address_mode >> 14)&0x3, (cmd->address_mode >> 6)&0x3);
    if (( (cmd->address_mode >> 14)&0x3)>((cmd->address_mode >> 6)&0x3))
        opsize = (cmd->address_mode >> 14)&0x3;
    else
        opsize = (cmd->address_mode >> 6)&0x3;
	
	
	int op3 = cmd->op[2];//����
	//0: == 1: != 2: >= 3:<= 4:> 5:< 6:&& 7:||
    //��������ñ�־λ1, ����������
	char msg[201] = "";
	switch (opsize)
	{
	case 0:
		switch (op3)
		{
		case 0: 
			if (*((char*)src) == *((char*)dest))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
		case 1: 
			if (*((char*)src) != *((char*)dest))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		case 2:	
			if (*((char*)dest) >= *((char*)src))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		case 3:	
			if (*((char*)dest) <= *((char*)src))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		case 4: 		
			if (*((char*)dest) > *((char*)src))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		case 5: 		
			if (*((char*)dest) < *((char*)src))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		case 6: 		//'&&'
			if (*((char*)dest) &&  *((char*)src))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		case 7:         //'||'
			if (*((char*)dest) || *((char*)src))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		default:
			{
				snprintf(msg, 200, "SE:: invalid test command condition: %d, (line = %d))", op3, cmd->line);
				nLOG(msg, 9);
				return FALSE;
			}
		};
		break;
	case 1:
		switch (op3)
		{
		case 0: 
			if (*((short*)src) == *((short*)dest))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
		case 1: 
			if (*((short*)src) != *((short*)dest))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		case 2:	
			if (*((short*)dest) >= *((short*)src))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		case 3:	
			if (*((short*)dest) <= *((short*)src))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		case 4: 		
			if (*((short*)dest) > *((short*)src))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		case 5: 		
			if (*((short*)dest) < *((short*)src))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		case 6: 		//'&&'
			if (*((short*)dest) &&  *((short*)src))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		case 7:         //'||'
			if (*((short*)dest) || *((short*)src))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		default:
			{
				snprintf(msg, 200, "SE:: invalid test command condition: %d, (line = %d))", op3, cmd->line);				
				nLOG(msg, 9);
				return FALSE;
			}
		};
		break;
	case 2:
		switch (op3)
		{
		case 0: 
			if (*((int*)src) == *((int*)dest))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
		case 1: 
			if (*((int*)src) != *((int*)dest))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		case 2:	
			if (*((int*)dest) >= *((int*)src))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		case 3:	
			if (*((int*)dest) <= *((int*)src))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		case 4: 		
			if (*((int*)dest) > *((int*)src))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		case 5: 		
			if (*((int*)dest) < *((int*)src))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		case 6: 		//'&&'
			if (*((int*)dest) &&  *((int*)src))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		case 7:         //'||'
			if (*((int*)dest) || *((int*)src))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		default:
			{
				snprintf(msg, 200, "SE:: invalid test command condition: %d, (line = %d))", op3, cmd->line);				
				nLOG(msg, 9);
				return FALSE;
			}
		};
		break;
	case 3:
		switch (op3)
		{
		case 0: 
			if (*((long*)src) == *((long*)dest))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
		case 1: 
			if (*((long*)src) != *((long*)dest))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		case 2:	
			if (*((long*)dest) >= *((long*)src))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		case 3:	
			if (*((long*)dest) <= *((long*)src))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		case 4: 		
			if (*((long*)dest) > *((long*)src))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		case 5: 		
			if (*((long*)dest) < *((long*)src))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		case 6: 		//'&&'
			if (*((long*)dest) &&  *((long*)src))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;
			
		case 7:         //'||'
			if (*((long*)dest) || *((long*)src))
				__PSW |= 0x1;
			else
				__PSW &= 0xfffe;
			break;						
		default:
			{
				snprintf(msg, 200, "SE:: invalid test command condition: %d, (line = %d))", op3, cmd->line);			
				nLOG(msg, 9);
				return FALSE;
			}
		};
		break;
	}
	__IP++;
	return TRUE;
}

BOOL CVirtualMachine::_ret(PCOMMAND cmd)
{
	char msg[201] = "";
	snprintf(msg, 200, "SE:: script '%s' exit at line %04d, IP %04d", m_pCurCall->pFunc->m_szName, cmd->line, __IP);
	
	//m_CallStack.back();
	if (m_pCurCall->VMemory)
		delete m_pCurCall->VMemory;
	m_CallStack.pop_back();
	if (m_CallStack.empty() == false)
		m_pCurCall = &m_CallStack.back();
	else
	{
		nLOG(msg, 85);
		m_pCurCall = NULL;
	//	IsRunning = FALSE;
		m_ToStop.Set();

		return TRUE;
	}
	
	__IP = m_pCurCall->IP;
	
	return TRUE;	
}


/**
����������	BOOL CVirtualMachine::Run()
�������ܣ�	��ʼ����vm
����˵����	
�� �� ֵ��	BOOL  - �ɹ�����TRUE�� ʧ�ܷ���FALSE
�� д �ˣ�	������
������ڣ�	2002-3-19
**/
BOOL CVirtualMachine::Run()
{

	m_Stopped.Reset();

	m_nRemainCmdNum = 0;
	__IP = 0;
	//IsRunning = TRUE;
	m_lDbgCmdID = DBGCMD_NULL;
	char msg[1001] = "";	
	
	int opcode;
	BOOL bError = FALSE;
	int nodebug = 0;
	PCOMMAND pcmd = NULL;
//	vector kk;
	//����ϵ��б�
//	m_BreakPoint = new long[m_pCurCall->pFunc->m_nCurrentCmdNum];
//	memset(m_BreakPoint, -1, sizeof(long)*m_pCurCall->pFunc->m_nCurrentCmdNum);
	m_BpList.clear();


	// start time
	m_StatusLock.Lock();
	GetLocalTime(&m_StartTime);	
	m_StatusLock.Unlock();

	while (m_ToStop.Wait(0) == LOCKEX_ERR_TIMEOUT && !bError)
	{
		m_StatusLock.Lock();
		opcode = m_pCurCall->pFunc->m_pCmdTable[__IP].opcode;
		pcmd = &(m_pCurCall->pFunc->m_pCmdTable[__IP]);
		if (m_nWorkMode >= VM_MODE_DEBUG)
		{

			if (__IP == 0 || (__IP > 0 && pcmd->line != m_pCurCall->pFunc->m_pCmdTable[__IP - 1].line ) )// �����ӡ��ͬ����
			{
				snprintf(msg, 200, "SE::%s IP: %d line: %d", m_pCurCall->pFunc->m_szName, __IP, pcmd->line);
				nLOG(msg, 500);
			}

			if (m_nWorkMode >= VM_MODE_STEPDEBUG)
			{
				m_pCurCall->pFunc->OutupCmd(__IP, msg);
				nLOG(msg, 500);
			}
		}
		switch (opcode)
		{
		case __mov:		
			if (!_mov(pcmd))	bError = TRUE;
			break;
		case __ret:
			if (!_ret(pcmd))	bError = TRUE;
			break;
		case __add:
			if (!_add(pcmd))	bError = TRUE;
			break;
		case __sub:
			if (!_sub(pcmd))	bError = TRUE;
			break;
		case __mul:
			if (!_mul(pcmd))	bError = TRUE;
			break;
		case __div:
			if (!_div(pcmd))	bError = TRUE;
			break;
		case __mod:
			if (!_mod(pcmd))	bError = TRUE;
			break;			
		case __fadd:
			if (!_fadd(pcmd))	bError = TRUE;
			break;
		case __fsub:
			if (!_fsub(pcmd))	bError = TRUE;
			break;
		case __fmul:
			if (!_fmul(pcmd))	bError = TRUE;
			break;
		case __fdiv:
			if (!_fdiv(pcmd))	bError = TRUE;
			break;
		case __not:
			if (!_not(pcmd))	bError = TRUE;
			break;
		case __notr:
			if (!_notr(pcmd)) bError = TRUE;
			break;
		case __jz:
			if (!_jz(pcmd))	bError = TRUE;
			break;
		case __jmp:
			if (!_jmp(pcmd))	bError = TRUE;
			break;
		case __test:
			if (!_test(pcmd))	bError = TRUE;
			break;
		case __jnz:
			if (!_jnz(pcmd))    bError = TRUE;
			break;
		case __callpub:
			if (!_callpub(pcmd))    bError = TRUE;
			break;
		case __parampub:
			if (!_parampub(pcmd))    bError = TRUE;
			break;
		case __endcallpub:
			try {			
				if (!_endcallpub(pcmd))    bError = TRUE;
			}
			catch(...)
			{
				// exit saftly befor throw
				m_Stopped.Set();
				throw;
			}
			break;
		case __callv:
			if (!_callv(pcmd))    bError = TRUE;
	//printf("SE:: debug info flag, %d@%s\n", __LINE__, __FILE__);
			break;
		case __paramv:
			if (!_paramv(pcmd))    bError = TRUE;
			break;
		case __endcallv:
			if (!_endcallv(pcmd))    bError = TRUE;
			break;
		case __ea:
			if (!_ea(pcmd)) bError = TRUE;
			break;
		case __cast:
			if (!_cast(pcmd)) bError = TRUE;
			break;
		default:
			snprintf(msg, 200, "SE:: can not find the implement of this command %xH (line: %d)", opcode, pcmd->line);
			nLOG(msg, 9);
			bError = TRUE;
			break;
		}

		m_StatusLock.Unlock();

		if (m_nWorkMode < VM_MODE_STEPDEBUG)
			continue;

		//--------- debug ---------//

		if (m_nRemainCmdNum > 0)
			m_nRemainCmdNum--;

		//�Ƿ��Ƕϵ�
		int index;
		for (index = 0; index < m_BpList.size(); index++)
		{
			if (m_BpList[index].lIP == __IP && strcmp(m_pCurCall->pFunc->m_szName, m_BpList[index].m_szName) == 0)
			{
				m_nRemainCmdNum = 0;
				break;
			}
		}
		if (index >= m_BpList.size())
		{
			// not a break point and remain command number > 0
			if (m_nRemainCmdNum > 0)
				continue;
		}
		//�Ƕϵ���Ƕϵ㵫m_nRemainCmdNum == 0
		//      ||
		//      \/
		//��Ⲣ���е�������

		m_bIsDebugBreak = TRUE;

		// clear all dbg input and output variable
		CVirtualMachine::m_DgbCmdLock.Lock();
		m_lDbgCmdID = DBGCMD_NULL;
		m_lDbgCmdP1 = m_lDbgCmdP2 = 0;
		memset(m_szDbgCmdP3, 0, 1024);
		CVirtualMachine::m_sDbgOutMsg = "";
		CVirtualMachine::m_lDbgOutCode = 0;

		CVirtualMachine::m_DgbCmdLock.Unlock();

		while (m_nRemainCmdNum == 0)
		{

			if (m_lDbgCmdID == DBGCMD_NULL)
			{
				Sleep(100);
				continue;
			}
			else
			{
				CVirtualMachine::m_DgbCmdLock.Lock();
				memset(msg, 0, 1001);
				long lDbgRet = 0;
		
				switch (m_lDbgCmdID)
				{
				case DBGCMD_NULL:	//û������	
					break;
				case DBGCMD_FORWARD:   	//��ǰִ��n����
					if (m_lDbgCmdP1 == 0)
						m_lDbgCmdP1 = 1;
					m_nRemainCmdNum = m_lDbgCmdP1;
					break;
				case DBGCMD_HALT: 	//ֹͣ����
				//	IsRunning = FALSE;
					m_ToStop.Set();
					m_nRemainCmdNum = 1;
					break;
				case DBGCMD_MEM:  	//��ʾ�ڴ�
					{
						long bytenum;//Ҫ��ʾ���ֽ���
						long bytestart;//��ʾ�Ŀ�ʼ�ֽ�
						char sTempMsg[201] = "";
//						int j;
						
						if (m_lDbgCmdP1 >= m_pCurCall->ulDataSegSize)
						{
							snprintf(msg, 1000, "start address %xH is too large\n", m_lDbgCmdP1);
							lDbgRet = -1;
							break;
						}
						if (m_lDbgCmdP1 <4) 
							m_lDbgCmdP1 = 4;

						bytestart = m_lDbgCmdP1;

						if (bytestart + m_lDbgCmdP2 >= m_pCurCall->ulDataSegSize)
						{
							bytenum = m_pCurCall->ulDataSegSize - bytestart;
						}
						else
							bytenum = m_lDbgCmdP2;
						for (int i = 0; i< bytenum; i++)
						{
							snprintf(sTempMsg, 200, " %02x", m_pCurCall->DataSeg[bytestart+i]);
							strcat(msg, sTempMsg);

							if (i%16 == 0 && i != 0) 
							{
								strcat(msg, "\r\n");
							}
						}
						break;////////break������������?
					}
				case DBGCMD_REG:				//��ʾ�Ĵ���
					{					
						strlwr(m_szDbgCmdP3);
						if (strlen(m_szDbgCmdP3) == 0)
						{//show all registers
							snprintf(msg, 1000, "PSW = %xH AX = %xH BX = %xH CX = %xH DX = %xH IP = %xH XX = %xH YX = %xH ZX = %xH\n", 
								m_registers[0], m_registers[1], m_registers[2], 
								m_registers[3], m_registers[4], m_registers[5], 
								m_registers[6], m_registers[6], m_registers[7]);
							break;							
						}
						switch(m_szDbgCmdP3[0])
						{
						case 'p':
								snprintf(msg, 1000, "PSW = %xH\n", m_registers[0]);break;
						case 'a':
								snprintf(msg, 1000, "AX = %xH\n", m_registers[1]);break;
						case 'b':
								snprintf(msg, 1000, "BX = %xH\n", m_registers[2]);break;
						case 'c': 	snprintf(msg, 1000, "CX = %xH\n", m_registers[3]);break;
						case 'd': 	snprintf(msg, 1000, "DX = %xH\n", m_registers[4]);break;						
						case 'i': 	snprintf(msg, 1000, "IP = %xH\n", m_registers[5]);break;
						case 'x': 	snprintf(msg, 1000, "XX = %xH\n", m_registers[6]);break;
						case 'y': 	snprintf(msg, 1000, "YX = %xH\n", m_registers[7]);break;
						case 'z': 	snprintf(msg, 1000, "ZX = %xH\n", m_registers[8]);break;
						default:	snprintf(msg, 1000, "no this register\n"); lDbgRet = -1;
						}
						break;
					}
				case DBGCMD_BP:   	//��ʾ���жϵ�
					{
						int i;
//						int count;
						char sTemp[201] = "";
					

						for (i = 0; i< m_BpList.size(); i++)
						{
							snprintf(sTemp, 200, "IP:%d LINE: %d\n", m_BpList[i].lIP, m_pCurCall->pFunc->m_pCmdTable[m_BpList[i].lIP].line);
							strcat(msg, sTemp);
						}
						break;
					}
				case DBGCMD_SP:   	//���öϵ�
					if (SetBreakPoint(m_lDbgCmdP1))
					{
						//snprintf(msg, 1000,"set break point success.\n");
					}
					else
					{
						//snprintf(msg, 1000, "set break point failed.\n");
						lDbgRet = -1;
					}
					break;
				case DBGCMD_DP:   //ɾ���ϵ�
					if (DelBreakPoint(m_lDbgCmdP1))
					{
						//	snprintf(msg, 1000, "delete break point success.\n");
					}
					else
					{
						snprintf(msg, 1000, "delete break point failed.\n");
						lDbgRet = -1;
					}
					break;
				case DBGCMD_GO://ȡ������
					m_nRemainCmdNum = 0xffffffff;
					break;
				default:
					{
						snprintf(msg, 1000, "invalid debug command: %d\n", m_lDbgCmdID);
						lDbgRet = -2;
					}
				}

				//�������
				m_lDbgCmdID = m_lDbgCmdP1 = m_lDbgCmdP1 = 0;
				memset(m_szDbgCmdP3, 0, 1024);
				CVirtualMachine::m_sDbgOutMsg = msg;
				CVirtualMachine::m_lDbgOutCode = lDbgRet;

				CVirtualMachine::m_DgbCmdLock.Unlock();

			}
		}
		m_bIsDebugBreak = FALSE;
/*		//set debug input		
		long dest;
		char input[1024];
		char incmd[20];
		BOOL bContinue = FALSE;
		if (m_nWorkMode >= VM_MODE_STEPDEBUG && !nodebug)
		{
			while (!bContinue)
			{
				memset(input, 0, 1024);
				memset(incmd, 0, 20);
				scanf("%s", input);
				int a;
				if (( a = sscanf(input, "%c %x", incmd, &dest)) < 2)
					sscanf(input, "%s", incmd);
				if (!strcmp(incmd, "r"))
				{
					switch(dest)
					{
					case 0: printf("PSW = %x\n", m_registers[dest]);break;
					case 1: printf("AX = %x\n", m_registers[dest]);break;
					case 2: printf("BX = %x\n", m_registers[dest]);break;
					case 3: printf("CX = %x\n", m_registers[dest]);break;
					case 4: printf("DX = %x\n", m_registers[dest]);break;
					case 5: printf("PSW = %x\n", m_registers[dest]);break;
					case 6: printf("IP = %x\n", m_registers[dest]);break;
					case 7: printf("XX = %x\n", m_registers[dest]);break;
					case 8: printf("YX = %x\n", m_registers[dest]);break;
					case 9: printf("ZX = %x\n", m_registers[dest]);break;
					default:printf("no this register\n");
					}
				}
				else if (!strcmp(incmd, "d"))
				{
					printf("%04x:", dest);
					for (int i = 0; i< 16; i++)
					{
						printf(" %02x", m_pCurCall->DataSeg[dest+i]);
					}
					printf("\r\n");
				}
				else if (!strcmp(incmd, "s"))
				{
					printf("%04x:", dest);
					for (int i = 0; i< 16; i++)
					{
						printf(" %02x", m_pCurCall->StaticSeg[dest+i]);
					}
					printf("\r\n");
				}
				else if (!strcmp(incmd, "g"))
				{
					bContinue = TRUE;
				}
				else if (!strcmp(incmd, "G"))
				{
					nodebug = 1;
					bContinue = TRUE;
				}
				else if (!strcmp(incmd, "m"))
				{
					printf("%04x:", dest);
					char temp;
					for (int i = 0; i< 16; i++)
					{
						
						_asm
						{
							mov EAX, [dest+i]
								mov temp, AH
						}
						
						printf(" %02x", temp);
					}
					printf("\r\n");
				}
				else
					printf("invalid command\r\n");
			}
			
		}*/
	}	

	m_Stopped.Set();
	return TRUE;
}



void CVirtualMachine::OutputMemToFile(FILE* file)
{
	if (file == NULL)
		return;
	
	if (m_pCurCall->pFunc == NULL)
		return;
	
	int i;
	char msg[1024];
	SYSTEMTIME time;
	GetLocalTime(&time);
	snprintf(msg, 1000, "%04d-%02d-%02d %02d:%02d:%0d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	fwrite(msg, 1, strlen(msg), file);
	snprintf(msg, 1000, "Function Name: %s\r\n"
	//	"Current Line: %d\r\n"
		"Current IP: %d\r\n"
		"virtual memory segment: %x\r\n"
		"virtual memory segment size: %d\r\n"
		"static segment: %x\r\n"
		"static segment size: %x\r\n"
		"Data segment: %x\r\n"
		"Data segment size: %x\r\n"
		"AX = %x(%d)\r\n"
		"BX = %x(%d)\r\n"
		"CX = %x(%d)\r\n"
		"DX = %x(%d)\r\n"
		"PSW = %x(%d)\r\n",
		m_pCurCall->pFunc->m_szName,
		//m_pCurCall->pFunc->m_pCmdTable[m_pCurCall->IP].line,
//		m_pCurCall->IP,
		__IP,
		m_pCurCall->VMemory,
		m_pCurCall->ulVMemSize,
		m_pCurCall->StaticSeg,
		m_pCurCall->ulSSize,
		m_pCurCall->DataSeg,
		m_pCurCall->ulDataSegSize,
		__AX, __AX, __BX, __BX, __CX, __CX, __DX, __DX, __PSW, __PSW
		);

	fwrite(msg, 1, strlen(msg), file);
	snprintf(msg, 1000, "************************variable result*********************\r\n");
	fwrite(msg, 1, strlen(msg), file);
	
	//output command table
	int offset;
	for (i = 0; i < m_pCurCall->pFunc->m_SymbolTable.m_nSymbolCount; i++)
	{
		offset = m_pCurCall->pFunc->m_SymbolTable.tableEntry[i].address;
		snprintf(msg, 1000, "%s:   %04x\r\n",m_pCurCall->pFunc->m_SymbolTable.tableEntry[i].szName, *(int*)(m_pCurCall->DataSeg+offset));
		fwrite(msg, 1, strlen(msg), file);
		
	}
	snprintf(msg, 1000, "************************static segment*********************\r\n");
	fwrite(msg, 1, strlen(msg), file);
	
	int j;
	for (offset = 0; offset < m_pCurCall->pFunc->m_nSSUsedSize; )
	{
		snprintf(msg, 1000,  "%04x:", offset);
		fwrite(msg, 1, strlen(msg), file);
		
		for (j = 0;j <8; j++)
		{
			snprintf(msg, 1000, " %02x", (BYTE)(m_pCurCall->VMemory[offset]));
			fwrite(msg, 1, strlen(msg), file);
			offset++;
		}
		snprintf(msg, 1000, " - ", offset);
		fwrite(msg, 1, strlen(msg), file);
		for (j = 0;j <8; j++)
		{
			snprintf(msg, 1000, " %02x", (BYTE)(m_pCurCall->VMemory[offset]));
			fwrite(msg, 1, strlen(msg), file);
			offset++;
		}
		snprintf(msg, 1000, "\r\n", offset);
		fwrite(msg, 1, strlen(msg), file);
		
	}
	snprintf(msg, 1000, "************************data segment*********************\r\n");
	fwrite(msg, 1, strlen(msg), file);
	
	for (offset = 0; offset < m_pCurCall->pFunc->m_SymbolTable.m_nTotalSize; )
	{
		snprintf(msg, 1000, "%04x:", offset);
		fwrite(msg, 1, strlen(msg), file);
		for (j = 0;j <8; j++)
		{
			snprintf(msg, 1000,  " %02x", (BYTE)(m_pCurCall->DataSeg[offset]));
			fwrite( msg, 1, strlen(msg), file);
			offset++;
		}

		snprintf(msg, 1000, " - ", offset);
		fwrite(msg, 1, strlen(msg), file);

		for (j = 0;j <8; j++)
		{
			snprintf(msg, 1000,  " %02x", (BYTE)(m_pCurCall->DataSeg[offset]));
			fwrite(msg, 1, strlen(msg), file);
			offset++;
		}
		snprintf(msg, 1000, "\r\n", offset);
		fwrite(msg, 1, strlen(msg), file);
	}	
}

BOOL CVirtualMachine::OutputMemToFile(char* szFileName)
{

	//BOOL bRet;
	FILE* file = NULL;
	file = fopen(szFileName, "wb");
	if (file == NULL)
		return FALSE;
	OutputMemToFile(file);	
	fclose(file);

	return TRUE;
}



/*
��������     : CVirtualMachine::AttachParam
��������	    : 
BYTE *pParam:  �����ĵ�ַ, ע�ⲻ�ǲ���������
int size_t  :  �������ݵĳ���
����˵��     : 
����ֵ       : 
��д��       : ������
�������     : 2001 - 4 - 27
*/
long CVirtualMachine::AttachParam(BYTE *pParam, int size_t)
{
	char sMsg[201] = "";
	if (pParam == NULL)
	{
		snprintf(sMsg, 200, "SE::AttachParam: input parameter block pointer is NULL, script: %s", m_pCurCall->pFunc->m_szName);
		nLOG(sMsg, 9);
		throw new CVMMException("SE::AttachParam: input parameter block pointer is NULL");
	//	return FALSE;
	}

	// modified on 20030331 by weihua ju
	// modified on 20030327 by weihua ju
	if (size_t != m_pCurCall->pFunc->m_iParamTotalSize)
	{
		snprintf(sMsg, 200, "SE::AttachParam: input parameter block size(%d) is bigger than function total parameters size(%d), script: %s", size_t, m_pCurCall->pFunc->m_iParamTotalSize, m_pCurCall->pFunc->m_szName);
		nLOG(sMsg, 9);
		return REQERR_PARAMNUMERROR;
	}
	memcpy(m_pCurCall->DataSeg, pParam, size_t);
	return REQERR_NOERROR;
}


/*
����������	BOOL CVirtualMachine::Preprocess1(PCOMMAND cmd, int &op1mode, int &op1reflvl, unsigned char* &dest, BOOL bValidate)
�������ܣ�	ָ��Ԥ��������������ָ�
����˵����	
			[IN]PCOMMAND cmd	-	ָ������
			[OUT]int &op1mode	-	������1��Ѱַ��ʽ��
			[OUT]int &op1reflvl	-	������1�ļ�Ӽ���
			[OUT]unsigned char* &dest	-	ʵ�ʵ�ַ	
			[IN]BOOL bValidate	-	�Ƿ���е�ַ��֤
�� �� ֵ��	BOOL  - �ɹ���ʧ��
�� д �ˣ�	������
������ڣ�	2001-6-26
*/
BOOL CVirtualMachine::Preprocess1(PCOMMAND cmd, int &op1mode, int &op1reflvl, unsigned char* &dest, BOOL bValidate)
{
	char msg[201] = "";
	int i;
	op1mode = (cmd->address_mode>>8)&3;
	op1reflvl = (cmd->address_mode>>12)&0x3;
	switch (op1mode)
	{
	case AMODE_DIRECT:
		dest = (unsigned char*)&(cmd->op[0]);break;
	case AMODE_MEM:
		if (cmd->op[0]>= m_pCurCall->ulDataSegSize || cmd->op[0]<0)
		{
			REPORT_ERROR("virtual machine error: access violation\r\n", 90);
			return FALSE;
		}
		dest = (unsigned char*)&(m_pCurCall->DataSeg[cmd->op[0]]);break;
	case AMODE_REG:
		if (cmd->op[0]> 8 || cmd->op[0]<0)
		{
			REPORT_ERROR("virtual machine error: access violation\r\n", 90);
			return FALSE;
		}
		dest = (unsigned char*)&(m_registers[cmd->op[0]]);break;
	case AMODE_STATIC:	
		if (cmd->op[0]>= m_pCurCall->ulSSize || cmd->op[0]<0)
		{
			REPORT_ERROR("virtual machine error: access violation\r\n", 90);
			return FALSE;
		}
		
		dest = (unsigned char*)&(m_pCurCall->StaticSeg[cmd->op[0]]);break;
//	case AMODE_ARRAY: 
	//	dest = (unsigned char*)&(cmd->op[0]);break;
		
		//	dest = (unsigned char*)(m_pCurCall->DataSeg[__BX+ m_pCurCall->DataSeg[cmd->op[0]]]);break;
		
	default:
		{
			snprintf(msg, 200, "invalid address mode, line %d, op %xH, addressmode %xH", cmd->line, cmd->op, cmd->address_mode);
			nLOG(msg, 50);
			return FALSE;
		}
	}
	
	if (bValidate&& op1mode != AMODE_DIRECT && !ValidateAddress(dest))
	{
		char cmdmsg[128];
		CFunction::OutCmd(cmd, cmdmsg);
		 snprintf(msg, 200, "access violation %s", cmdmsg);
		 nLOG(msg, 20);

		return FALSE;
	}

	for (i = 0; i< op1reflvl; i++)
	{
		//dest = &m_pCurCall->DataSeg[*dest];
		dest = (unsigned char*)(*((int*)dest));
		if (bValidate&&!ValidateAddress(dest))
		{
			char cmdmsg[128];
			CFunction::OutCmd(cmd, cmdmsg);

			snprintf(msg, 200, "access violation %s", cmdmsg);
			nLOG(msg, 20);
			return FALSE;
		}
	}

	return TRUE;
	
}

BOOL CVirtualMachine::Preprocess2(PCOMMAND cmd, int &op1mode, int &op2mode, int &op1reflvl, int &op2reflvl, unsigned char * &dest, unsigned char* &src)
{
	char msg[201] = "";
	int i;
	op1mode = (cmd->address_mode>>8)&0x3;
	op2mode = cmd->address_mode&0x0003;
	op2reflvl = (cmd->address_mode>>4)&0x3;
	op1reflvl = (cmd->address_mode>>12)&0x3;
	
	switch (op1mode)
	{
	case AMODE_DIRECT:
		dest = (unsigned char*)&(cmd->op[0]);break;
	case AMODE_MEM:
		if (cmd->op[0]>= m_pCurCall->ulDataSegSize || cmd->op[0]<0)
		{
			REPORT_ERROR("virtual machine error: access violation\r\n", 90);
			return FALSE;
		}
		dest = (unsigned char*)&(m_pCurCall->DataSeg[cmd->op[0]]);break;
	case AMODE_REG:
		if (cmd->op[0]> 8 || cmd->op[0]<0)
		{
			REPORT_ERROR("virtual machine error: access violation\r\n", 90);
			return FALSE;
		}
		dest = (unsigned char*)&(m_registers[cmd->op[0]]);break;
	case AMODE_STATIC:		
		if (cmd->op[0]>= m_pCurCall->ulSSize || cmd->op[0]<0)
		{
			REPORT_ERROR("virtual machine error: access violation", 90);
			return FALSE;
		}
		dest = &(m_pCurCall->StaticSeg[cmd->op[0]]);break;
/*	case AMODE_ARRAY: 
		dest = (unsigned char*)&(cmd->op[0]);break;
		//		dest = (unsigned char*)(m_pCurCall->DataSeg[__BX+ m_pCurCall->DataSeg[cmd->op[0]]]);break;
	*/	


	default:
		{			
			snprintf(msg, 200, "SE:: invalid address mode, line %d, op %xH, addressmode %xH", cmd->line, cmd->op, cmd->address_mode);
			nLOG(msg, 9);
			return FALSE;
		}
	}
	if (op1mode != AMODE_DIRECT&&!ValidateAddress(dest))
	{
		char cmdmsg[128];
		CFunction::OutCmd(cmd, cmdmsg);

		snprintf(msg, 200, "access violation (line: %d) %s", cmd->line, cmdmsg);
		nLOG(msg, 9);

		return FALSE;
	}

	for ( i = 0; i< op1reflvl; i++)
	{
		//	dest = &m_pCurCall->DataSeg[*((int*)dest)];
		dest = (unsigned char *)(*((int*)dest));
		if (!ValidateAddress(dest))
		{	
			char cmdmsg[128];
			CFunction::OutCmd(cmd, cmdmsg);
			
			snprintf(msg, 200, "SE:: access violation (line: %d) ", cmd->line, cmdmsg);
			nLOG(msg, 9);

			return FALSE;
		}
		
	}

	switch (op2mode)
	{
	case AMODE_DIRECT:
		src = (unsigned char*)&(cmd->op[1]);break;
	case AMODE_MEM:
		if (cmd->op[1]>= m_pCurCall->ulDataSegSize || cmd->op[1]<0)
		{
			REPORT_ERROR("virtual machine error: access violation\r\n", 90);
			return FALSE;
		}
		
		src = (unsigned char*)&(m_pCurCall->DataSeg[cmd->op[1]]);break;
	case AMODE_REG:
		if (cmd->op[1]> 8 || cmd->op[1]<0)
		{
			REPORT_ERROR("virtual machine error: access violation\r\n", 90);
			return FALSE;
		}
		
		src = (unsigned char*)&(m_registers[cmd->op[1]]);break;
	case AMODE_STATIC:
		if (cmd->op[1]>= m_pCurCall->ulSSize || cmd->op[1]<0)
		{
			REPORT_ERROR("virtual machine error: access violation\r\n", 90);
			return FALSE;
		}
		
		src = (unsigned char*)&(m_pCurCall->StaticSeg[cmd->op[1]]);break;
//	case AMODE_ARRAY: 
	//	src = (unsigned char*)&(cmd->op[1]);break;
		
		//		src = (unsigned char*)(m_pCurCall->DataSeg[__BX+ m_pCurCall->DataSeg[cmd->op[1]]]);break;
	default:
		{			
			snprintf(msg, 200, "SE:: invalid address mode, line %d, op %xH, addressmode %xH", cmd->line, cmd->op, cmd->address_mode);
			nLOG(msg, 9);
			return FALSE;
		}
	}
	
	if (op2mode != AMODE_DIRECT && !ValidateAddress(src))
	{
		char cmdmsg[128];
		CFunction::OutCmd(cmd, cmdmsg);
		
		snprintf(msg, 200, "SE:: access violation (line: %d) %s", cmd->line, cmdmsg);
		nLOG(msg, 9);
		return FALSE;
	}
	for ( i = 0; i< op2reflvl; i++)
	{
		//src = &m_pCurCall->DataSeg[*((int*)src)];
		src = (unsigned char*)(*((int*)src));
		if (!ValidateAddress(src))
		{
			char cmdmsg[128];
			CFunction::OutCmd(cmd, cmdmsg);
			
			snprintf(msg, 200, "SE:: access violation (line: %d) ", cmd->line, cmdmsg);
			nLOG(msg, 9);
			return FALSE;
		}
	}
	return TRUE;
}


void CVirtualMachine::SetWorkMode(int mode)
{
	m_nWorkMode = mode;
}


/*
����������	BOOL CVirtualMachine::ValidateAddress(unsigned char* address)
�������ܣ�	��֤��ַ�Ƿ�Ϸ�
����˵����	
			[IN]unsigned char* address	-	��ַ
�� �� ֵ��	BOOL  - �ɹ���ʧ��
�� д �ˣ�	������
������ڣ�	2001-6-26
*/
BOOL CVirtualMachine::ValidateAddress(unsigned char* address)
{
	CALLSTACKELE* pCall = NULL;
	// CALL_STACK  stack = m_CallStack;
	long s = m_CallStack.size();
	int i = 0;
	for (; i < s; i++ )
	{
		pCall = &(m_CallStack[i]);
		if (address>= pCall->VMemory && address < pCall->VMemory + pCall->ulVMemSize)
			return TRUE;
	}

	if (address >= (unsigned char*)&m_registers[0] && address < (unsigned char*)&m_registers[LASTREGISTER])
		return TRUE;
	
	for ( i = 0; i< m_lExternalSpaceNum; i++)
	{
		if (address >= (unsigned char*)m_pExternalSpaceList[i].ulExternalSpaceAddress && address < (unsigned char*)m_pExternalSpaceList[i].ulExternalSpaceAddress + m_pExternalSpaceList[i].ulExternalSpaceSize)
			return TRUE;
	}
	
	return FALSE;
}

void CVirtualMachine::AddExternalSpace(unsigned char *start, unsigned long size)
{
	if (m_lExternalSpaceListSize == m_lExternalSpaceNum)
	{
		EXTERNALSPACE* pTemp = new EXTERNALSPACE[m_lExternalSpaceListSize + 10];
		if (pTemp == NULL)
		{
			//REPORT_MEM_ERROR("not enough memory when virtual machine alloc external space description list\r\n")
			throw new CVMMException("not enough memory when virtual machine alloc external space description list");
		//	return FALSE;
		}
		memcpy(pTemp, m_pExternalSpaceList, sizeof(EXTERNALSPACE)*  m_lExternalSpaceNum);
		delete m_pExternalSpaceList;
		m_pExternalSpaceList = pTemp;
		m_lExternalSpaceListSize += 10;
	}
	m_pExternalSpaceList[m_lExternalSpaceNum].ulExternalSpaceAddress = (long)start;
	m_pExternalSpaceList[m_lExternalSpaceNum].ulExternalSpaceSize = size;
	m_lExternalSpaceNum++;
//	return TRUE;
}

BOOL CVirtualMachine::_cast(PCOMMAND cmd)
{
	char msg[201] = "";
	CMD_PREPROCESS2
	long op3 = cmd->op[2];//ת������������
	switch (op3)
	{
		//integer -> float
	case __CAST_L2F:
	case __CAST_I2F:
		*(float*)dest = (float)(*(long*)src);
		break;
	case __CAST_C2F:
		*(float*)dest = (float)(*(char*)src);		
		break;
	case __CAST_S2F:
		*(float*)dest = (float)(*(short*)src);		
		break;

		//* -> char
	case __CAST_S2C:
		*(char*)dest = (char)(*(short*)src);
		break;
	case __CAST_L2C:
	case __CAST_I2C:
		*(char*)dest = (char)(*(long*)src);
		break;
	
		//* -> short
	case __CAST_C2S:
		*(short*)dest = (short)(*(char*)src);
		break;
	case __CAST_I2S:
	case __CAST_L2S:
		*(short*)dest = (short)(*(long*)src);
		break;

		//* -> long||int
	case __CAST_C2L:
	case __CAST_C2I:
		*(long*)dest = (long)(*(char*)src);
		break;
	case __CAST_S2L:
	case __CAST_S2I:
		*(long*)dest = (long)(*(short*)src);
		break;
		//long -> int
	case __CAST_L2I:
	case __CAST_I2L:
		*(long*)dest = (*(long*)src);
		break;

	default:
		
		snprintf(msg, 200, "SE:: cast command with invalid cast type code (line = %d)", cmd->line);
		nLOG(msg, 9);
		return  FALSE;
		break;
	}
	__IP++;
	return TRUE;	
}

BOOL CVirtualMachine::_fadd(PCOMMAND cmd)
{
	char msg[201] = "";
	CMD_PREPROCESS2
	short opsize;//���ֽڲ���, �ֲ���, ��˫�ֲ���
	//	opsize= __max( (cmd->address_mode >> 14)&0x3, (cmd->address_mode >> 6)&0x3);
    if (( (cmd->address_mode >> 14)&0x3)>((cmd->address_mode >> 6)&0x3))
        opsize = (cmd->address_mode >> 14)&0x3;
    else
        opsize = (cmd->address_mode >> 6)&0x3;
	float temp;
	switch (opsize)
	{
		case 2:	
			temp = *((float*)dest) + *((float*)src);
			__AX = *(long*)(&temp);
			break;
		default:			
			snprintf(msg, 200, "invalid operation size(line = %d)", cmd->line);
			nLOG(msg,  9);
			return FALSE;
	}
	__IP++;
	return TRUE;
}
BOOL CVirtualMachine::_fsub(PCOMMAND cmd)
{
	CMD_PREPROCESS2
	short opsize;//���ֽڲ���, �ֲ���, ��˫�ֲ���
//	opsize= __max( (cmd->address_mode >> 14)&0x3, (cmd->address_mode >> 6)&0x3);
    if (( (cmd->address_mode >> 14)&0x3)>((cmd->address_mode >> 6)&0x3))
        opsize = (cmd->address_mode >> 14)&0x3;
    else
        opsize = (cmd->address_mode >> 6)&0x3;
	float temp;
	switch (opsize)
	{
		case 2:	
			temp = *((float*)dest) - *((float*)src);
			__AX = *(long*)&temp;
			break;
		default:
			char msg[201] = "";
			snprintf(msg, 200, "SE:: invalid operation size(line = %d)", cmd->line);
			nLOG(msg,  9);
			return FALSE;
	}

	__IP++;
	return TRUE;
}
BOOL CVirtualMachine::_fmul(PCOMMAND cmd)
{
	CMD_PREPROCESS2
	short opsize;//���ֽڲ���, �ֲ���, ��˫�ֲ���
//	opsize= __max( (cmd->address_mode >> 14)&0x3, (cmd->address_mode >> 6)&0x3);
    if (( (cmd->address_mode >> 14)&0x3)>((cmd->address_mode >> 6)&0x3))
        opsize = (cmd->address_mode >> 14)&0x3;
    else
        opsize = (cmd->address_mode >> 6)&0x3;
	float temp;
	switch (opsize)
	{
		case 2:
			temp = *((float*)dest) * *((float*)src);
			__AX = *(long*)&temp;break;
		default:
			char msg[201] = "";
			snprintf(msg, 200, "SE:: invalid operation size(line = %d)", cmd->line);
			nLOG(msg,  9);
			return FALSE;
	}

	__IP++;
	return TRUE;
}
BOOL CVirtualMachine::_fdiv(PCOMMAND cmd)
{
	char szMsg[201] = "";
	CMD_PREPROCESS2
	short opsize;//���ֽڲ���, �ֲ���, ��˫�ֲ���
//	opsize= __max( (cmd->address_mode >> 14)&0x3, (cmd->address_mode >> 6)&0x3);
    if (( (cmd->address_mode >> 14)&0x3)>((cmd->address_mode >> 6)&0x3))
        opsize = (cmd->address_mode >> 14)&0x3;
    else
        opsize = (cmd->address_mode >> 6)&0x3;
	float temp;
	switch (opsize)
	{
		case 2:
			if (*((float*)src) ==0)
			{
			snprintf(szMsg, 200, "VM:: devide zero error, function='%s', line=%d, IP=%d\r\n", m_pCurCall->pFunc->m_szName, cmd->line, __IP);
			REPORT_ERROR(szMsg, 20);
				return FALSE;
			}
			temp = *((float*)dest) / *((float*)src);
			__AX = *(long*)&temp;break;
		default:

			
			snprintf(szMsg, 200, "invalid operation size(line = %d)", cmd->line);
			nLOG(szMsg,  9);
			return FALSE;
	}
	__IP++;
	return TRUE;
}


BOOL CVirtualMachine::_loadlib(PCOMMAND cmd){
	char szMsg[201] = "";
	CMD_PREPROCESS1
	
	char szLibFile[_MAX_PATH] = "";
	strcpy(szLibFile, (const char*)dest);
#ifdef WIN32
	strcat(szLibFile, ".dll");
#else
	strcat(szLibFile, ".so");
#endif
	// TODO load library in pubfunction table

	__IP++;
	return TRUE;
}

/*
   ��������     : CVirtualMachine::SetDbgCmd
   ��������	    : �����������
   ����˵��     : 
   long lCmdID  : debug�����ID
   long p1      : ����1
   long p2      : ����2
   char* p3		: ����3(���Ȳ�����1024byte, ����\0)
   ����ֵ       : ��
   ��д��       : ������
   �������     : 2001 - 6 - 12
*/
void CVirtualMachine::SetDbgCmd(long lCmdID, long p1, long p2, char* p3)
{
	m_lDbgCmdID = lCmdID;
	m_lDbgCmdP1 = p1;
	m_lDbgCmdP2 = p2;
	strcpy(m_szDbgCmdP3, p3);
}





/*
   ��������     : CVirtualMachine::SetBreakPoint
   ��������	    : 
   ����˵��     : 
   ����ֵ       : 
   ��д��       : ������
   �������     : 2001 - 6 - 13
*/
BOOL CVirtualMachine::SetBreakPoint(long line)
{
//	if (m_BreakPoint == NULL)
//		return FALSE;

	long IP;
	long i;

	//�õ����е�һ��ָ���IP
	for (i = 0; i< m_pCurCall->pFunc->m_nCurrentCmdNum; i++)
	{
		if (m_pCurCall->pFunc->m_pCmdTable[i].line > line)
			return FALSE;
		if (m_pCurCall->pFunc->m_pCmdTable[i].line == line)
		{
			IP = i;
			break;
		}
	}

	//IP�Ƿ���ָ������Χ��
	if (IP >= m_pCurCall->pFunc->m_nCurrentCmdNum)
		return FALSE;

	//�����Ƿ��ظ�
	for (i = 0; i< m_BpList.size(); i++)
	{
		if (m_BpList[i].lIP == IP)
			return FALSE;
	}
	
	BP bp;
	bp.lIP = IP;
	strcpy(bp.m_szName, m_pCurCall->pFunc->m_szName);
	//	m_BpList[i].lIP = IP;
	//	m_BpList.size() = i + 1;
	m_BpList.push_back(bp);

	return TRUE;
}

/*
   ��������     : CVirtualMachine::DelBreakPoint
   ��������	    : 
   ����˵��     : 
   ����ֵ       : 
   ��д��       : ������
   �������     : 2001 - 6 - 13
*/
BOOL CVirtualMachine::DelBreakPoint(long line)
{
//	if (m_BreakPoint == NULL)
//		return	FALSE;
	long i;
	for (i = 0; i< m_BpList.size(); i++)
	{
		if (m_pCurCall->pFunc->m_pCmdTable[m_BpList[i].lIP].line == line)
		{
		//	memcpy(&m_BpList[i].lIP, &m_BreakPoint[i+1], sizeof(long)*(m_BpList.size() - i -1));
		//	m_BreakPoint[m_BpList.size() - 1] = -1;
		//	m_BpList.size() --;
			m_BpList.erase(m_BpList.begin() + i);
			return TRUE;
		}
	}	
	return FALSE;
}

void CVirtualMachine::DumpToFile()
{
	char sMsg[201] = "";
	///////////////////////// dmp memory to file /////////////////////////
	char sDmpFile[_MAX_PATH]  = "";
	time_t tm;
	time(&tm);
	struct tm* local_time = localtime(&tm);
	snprintf(sDmpFile, 200, 
		"/SEDMP%04d%02d%02d.%02d%02d%02d", 
		local_time->tm_year, 
		local_time->tm_mon,
		local_time->tm_mday, 
		local_time->tm_hour,
		local_time->tm_min,
		local_time->tm_sec);
	
	if (!OutputMemToFile(sDmpFile))
	{
		snprintf(sMsg, 200, "SE:: dump VM memroy to file %s failed", sDmpFile);
		nLOG(sMsg, 0);		
	}
	else
	{
		snprintf(sMsg, 200, "SE:: dump VM memroy to file %s ok", sDmpFile);
		nLOG(sMsg, 0);		
	}
	///////////////////////////////////////////////////////////////////////////
}

void CVirtualMachine::GetStatus(VMSTATUS* status)
{
		if (status == NULL)
			return;

		m_StatusLock.Lock();
		
		CALLSTACKELE* pCurCall = NULL;
		int i = 0;
		for (i = 0 ; i< m_CallStack.size(); i++)
		{
			pCurCall = &(m_CallStack[i]);
			status->sCallStack.push_back(pCurCall->pFunc->m_szName);
			if (i==0)
				status->lines.push_back(m_pCurCall->pFunc->m_pCmdTable[m_pCurCall->IP].line);
			else
				status->lines.push_back(pCurCall->pFunc->m_pCmdTable[pCurCall->IP].line);
			
		}
		m_StatusLock.Unlock();
}

CObjectInst* CVirtualMachine::LoadObject(CClassDes* c){
	// lookup regestered class instance
	CClass* pClass = m_classTable.getClass(c->GetFullName());
	if (pClass == NULL)
		pClass = m_classTable.createClassInst(c);

	// create object
	CObjectInst* obj = m_objTable.createObjectInstance(pClass);
	
	// load "create" method
	//long index = 1;
	CFunction* pfn = c->getMethod("create");
	LoadFunction(pfn);
	void* pthis = this;
	AttachParam((BYTE*)&pthis, sizeof(int));
	return obj;

}