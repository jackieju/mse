#ifndef __VIRTUALMACHINE_H__
#define __VIRTUALMACHINE_H__
// VirutalMachine.h: interface for the CVirtualMachine class.
//
//////////////////////////////////////////////////////////////////////
#include "ostime.h"
#include "ClassTable.h"
#include "ObjectInst.h"
#include "ObjTable.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <stack>
struct BP{
		char m_szName[21];					//函数名称
		long lIP;
};
typedef std::vector<BP>	bp_list;
struct	VMSTATUS;
//struct	VMSTATUS
//{
//	std::vector<std::string>	sCallStack;
//	std::vector<long>		lines;
//		VMSTATUS()
//		{
//			sCallStack.reserve(10);
//			lines.reserve(10);
//		}
//};
//
typedef struct _tagExternalSpace
{
	unsigned long ulExternalSpaceSize;     
	unsigned long ulExternalSpaceAddress;

}EXTERNALSPACE;

typedef struct {
	CFunction* pFunc;	// saved function pointer
	long IP;			// saved IP
	BYTE* VMemory;		// saved virtual memory
	BYTE* DataSeg;		//数据段
	BYTE* StaticSeg;	//常量段
	long ulVMemSize;
	long ulDataSegSize;
	long ulSSize;
}CALLSTACKELE;
typedef struct {
	long fn;
	PUBFUNCPARAM paramhdr;
	PUBFUNCPARAM *paramPt;
	long lParamBlockSize;
}FUNCTIONSTACKELE;
#pragma warning(disable:4786)
using namespace std;
typedef vector<CALLSTACKELE> CALL_STACK;
typedef stack<FUNCTIONSTACKELE*> FUNC_STACK;

/**
	对象名称	: CVirtualMachine
	对象描述	: 虚拟机类
	重要函数	: 
	///////////////////////////////////////////
	//操作虚拟机
	//添加外部可访问地址空间
	BOOL AddExternalSpace(unsigned char* start, unsigned long size);
	//传入参数
	BOOL AttachParam(BYTE* pParam, int size_t);
	//运行虚拟机
	BOOL Run();
	//加载要运行的函数
	BOOL LoadFunction(CFunction* pFunc);
	//虚拟机reset
	void Reset();

	////////////////////////////////////
	//以下成员用于调试
	//
	void SetWorkMode(int mode);
	void SetDbgCmd(long lCmdID, long p1, long p2, char* p3);
	void SetDbgOut(FILE* dbgout){if (dbgout) m_debugOut = dbgout;};
	void SetInputMethod(void (*pInputFn)(long &lCmdID, long& p1, long& p2, char* p3));
	void OutputMemToFile(char* szFileName);
	void OutputMemToFile(FILE* file);

	//虚拟机运行函数
	BOOL _test(PCOMMAND cmd);
	BOOL _jmp(PCOMMAND cmd);
	BOOL _jz(PCOMMAND cmd);
	BOOL _add(PCOMMAND cmd);
	BOOL _sub(PCOMMAND cmd);
	BOOL _mul(PCOMMAND cmd);
	BOOL _div(PCOMMAND cmd);
	BOOL _mod(PCOMMAND cmd);
	BOOL _fadd(PCOMMAND cmd);
	BOOL _fsub(PCOMMAND cmd);
	BOOL _fmul(PCOMMAND cmd);
	BOOL _fdiv(PCOMMAND cmd);
	BOOL _not(PCOMMAND cmd);
	BOOL _notr(PCOMMAND cmd);
	BOOL _mov(PCOMMAND cmd);
	BOOL _ret();
	BOOL _jmpz(PCOMMAND cmd);
	BOOL _jnz(PCOMMAND cmd);
	BOOL _callpub(PCOMMAND cmd);
	BOOL _parampub(PCOMMAND cmd);
	BOOL _endcallpub(PCOMMAND cmd);
	BOOL _ea(PCOMMAND cmd);

	编写人		: 居卫华
	完成日期	: 2001 - 8 - 21
**/

class CFunction;
class CVirtualMachine  
{
private:
	CSS_EVENT m_ToStop;
	CSS_EVENT m_Stopped;
	CSS_MUTEX m_StatusLock;
	SYSTEMTIME	m_StartTime;

//	CFunction* m_pMainFunc;
//	BOOL IsRunning;		// Is this VM in the procedure of running a function
	BOOL m_bIsDebugBreak;	// Dose the VM break when debuging 

	// 用来记录调用的函数名, 和参数
	FUNC_STACK m_FuncStack;


	// 虚拟机的CALL STACK, 记录脚本函数的调用过程
	CALL_STACK m_CallStack;
	CALLSTACKELE* m_pCurCall;
	//寄存器
#define REGNUM 6
	long m_registers[REGNUM];
#define __AX m_registers[1]
#define __BX m_registers[2]    
#define __CX m_registers[3]
#define __DX m_registers[4]
#define __IP m_registers[5]
#define __PSW m_registers[0]
//#define __XX m_registers[6] //存放pubfunction的pfn指针
//#define __YX m_registers[7] //存放pubfunction的参数指针
//#define __ZX m_registers[8] //存放pubfunction的参数个数
#define LASTREGISTER 5

	//内存
//	BYTE* m_DataSeg;  //数据段
//	BYTE* m_StaticSeg;//常量段
//	BYTE* m_VMemory;  //总的虚拟内存
	/*
	|-------------------------------|  <-m_staticSeg <-m_VMemory 
	|           常量段            	    |
	|-------------------------------|  <-m_DataSeg;
	|                               |
	|                               |
	|           数据段                 |
	|                               |
	|                               |
	|-------------------------------|
    */

//	unsigned long m_ulSSize;//常量段大小
//	unsigned long m_ulDataSegSize;//数据段大小
//	unsigned long m_ulVMemSize;//虚拟内存大小

	//外部可访问地址空间
	EXTERNALSPACE *m_pExternalSpaceList;
	long m_lExternalSpaceNum;
	long m_lExternalSpaceListSize;

	//以下成员变量用于调试
//工作模式
#define VM_MODE_NORMAL 0
#define VM_MODE_DEBUG  1
#define VM_MODE_STEPDEBUG 2

	long m_nWorkMode;           //运行模式
//	FILE* m_debugOut;			//调试命令的输出
	long m_nRemainCmdNum;       //连续执行的指令数

	
//	void (*m_pInputFn)(long &lCmdID, long& p1, long& p2, char* p3);//输入函数
//	long* m_BreakPoint;         //断点列表
//	long m_lBreakPtNum;         //断点个数

	// breakpoint list
	bp_list	m_BpList;

public:
	void GetStartTime(SYSTEMTIME* time)
	{
		if (time)
			memcpy(time, &m_StartTime, sizeof(SYSTEMTIME));
	}
	void GetStatus(VMSTATUS* status);
	CFunction* GetCurFunc()
	{
		if (m_pCurCall)
			return m_pCurCall->pFunc;
		else
			return NULL;
	}
	BOOL GetReg(long lIndex, long &reg)
	{
		reg = 0;
		if (lIndex < 0 || lIndex >= REGNUM)
		{
			return FALSE;
		}
		reg = m_registers[lIndex];
		return TRUE;
	};
	static long m_lDbgCmdID;			//debug command id
	static long m_lDbgCmdP1;			//debug command parameter 1
	static long m_lDbgCmdP2;			//debug command parameter 2
	static char m_szDbgCmdP3[1024];		//debug command parameter 3
	static std::string m_sDbgOutMsg;	// return msg of debug command execution
	static long m_lDbgOutCode;			// return code of debug command execution
										// 0: ok 
										// -1: failed
										

	static CSS_MUTEX m_DgbCmdLock; // debug command lock
	CVirtualMachine();
	virtual ~CVirtualMachine();

	//添加外部可访问地址空间
	void AddExternalSpace(unsigned char* start, unsigned long size);

	//传入参数
	long AttachParam(BYTE* pParam, int size_t);
	BOOL Run();
	void LoadFunction(CFunction* pFunc);
	void Reset();

	// for class$object loading
	CClassTable m_classTable;  // loaded class instance
	CObjTable m_objTable;		// loaded Objects
	CObjectInst*  LoadObject(CClassDes* c);

	//以下成员用于调试
	void SetWorkMode(int mode);
	void SetDbgCmd(long lCmdID, long p1, long p2, char* p3);
//	void SetDbgOut(FILE* dbgout){if (dbgout) m_debugOut = dbgout;};
	BOOL OutputMemToFile(char* szFileName);
	void OutputMemToFile(FILE* file);
	void DumpToFile();
	BOOL IsDebugBreak(){return m_bIsDebugBreak;};

	
private:

	void _LoadFunc(CFunction *pFunc);
	
	//设置和删除断点
	BOOL SetBreakPoint(long line);
	BOOL DelBreakPoint(long line);
	
	//验证地址是否在可用地址范围内
	//可用地址空间: 整个虚拟机 的虚拟内存, 寄存器, 请求数据块和外部可用地址空间
	BOOL ValidateAddress(unsigned char* address);
	//预处理双字节指令
	BOOL Preprocess2(PCOMMAND cmd, int &op1mode, int &op2mode, int &op1reflvl, int &op2reflvl, unsigned char * &dest, unsigned char* &src);//
	//预处理单字节指令
	BOOL Preprocess1(PCOMMAND cmd, int &op1mode, int &op1reflvl, unsigned char* &dest,  BOOL bValidate = TRUE);
//	void CallPubFunc(void* pParamBlock);

//command implement
	BOOL _test(PCOMMAND cmd);
	BOOL _jmp(PCOMMAND cmd);
	BOOL _jz(PCOMMAND cmd);
	BOOL _add(PCOMMAND cmd);
	BOOL _sub(PCOMMAND cmd);
	BOOL _mul(PCOMMAND cmd);
	BOOL _div(PCOMMAND cmd);
	BOOL _mod(PCOMMAND cmd);
	BOOL _fadd(PCOMMAND cmd);
	BOOL _fsub(PCOMMAND cmd);
	BOOL _fmul(PCOMMAND cmd);
	BOOL _fdiv(PCOMMAND cmd);
	BOOL _not(PCOMMAND cmd);
	BOOL _notr(PCOMMAND cmd);
	BOOL _mov(PCOMMAND cmd);
	BOOL _ret(PCOMMAND cmd);
	BOOL _jmpz(PCOMMAND cmd);
	BOOL _jnz(PCOMMAND cmd);
	BOOL _callpub(PCOMMAND cmd);
	BOOL _parampub(PCOMMAND cmd);
	BOOL _endcallpub(PCOMMAND cmd);
	BOOL _callv(PCOMMAND cmd);
	BOOL _paramv(PCOMMAND cmd);
	BOOL _endcallv(PCOMMAND cmd);
	BOOL _ea(PCOMMAND cmd);
	BOOL _cast(PCOMMAND cmd);
	BOOL _loadlib(PCOMMAND cmd);  // not used
	BOOL _eaobj(PCOMMAND cmd);    // get address for object/member
};


#endif //__VIRTUALMACHINE_H__
