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
		char m_szName[21];					//��������
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
	BYTE* DataSeg;		//���ݶ�
	BYTE* StaticSeg;	//������
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
	��������	: CVirtualMachine
	��������	: �������
	��Ҫ����	: 
	///////////////////////////////////////////
	//���������
	//����ⲿ�ɷ��ʵ�ַ�ռ�
	BOOL AddExternalSpace(unsigned char* start, unsigned long size);
	//�������
	BOOL AttachParam(BYTE* pParam, int size_t);
	//���������
	BOOL Run();
	//����Ҫ���еĺ���
	BOOL LoadFunction(CFunction* pFunc);
	//�����reset
	void Reset();

	////////////////////////////////////
	//���³�Ա���ڵ���
	//
	void SetWorkMode(int mode);
	void SetDbgCmd(long lCmdID, long p1, long p2, char* p3);
	void SetDbgOut(FILE* dbgout){if (dbgout) m_debugOut = dbgout;};
	void SetInputMethod(void (*pInputFn)(long &lCmdID, long& p1, long& p2, char* p3));
	void OutputMemToFile(char* szFileName);
	void OutputMemToFile(FILE* file);

	//��������к���
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

	��д��		: ������
	�������	: 2001 - 8 - 21
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

	// ������¼���õĺ�����, �Ͳ���
	FUNC_STACK m_FuncStack;


	// �������CALL STACK, ��¼�ű������ĵ��ù���
	CALL_STACK m_CallStack;
	CALLSTACKELE* m_pCurCall;
	//�Ĵ���
#define REGNUM 6
	long m_registers[REGNUM];
#define __AX m_registers[1]
#define __BX m_registers[2]    
#define __CX m_registers[3]
#define __DX m_registers[4]
#define __IP m_registers[5]
#define __PSW m_registers[0]
//#define __XX m_registers[6] //���pubfunction��pfnָ��
//#define __YX m_registers[7] //���pubfunction�Ĳ���ָ��
//#define __ZX m_registers[8] //���pubfunction�Ĳ�������
#define LASTREGISTER 5

	//�ڴ�
//	BYTE* m_DataSeg;  //���ݶ�
//	BYTE* m_StaticSeg;//������
//	BYTE* m_VMemory;  //�ܵ������ڴ�
	/*
	|-------------------------------|  <-m_staticSeg <-m_VMemory 
	|           ������              |
	|-------------------------------|  <-m_DataSeg;
	|                               |
	|                               |
	|           ���ݶ�              |
	|                               |
	|                               |
	|-------------------------------|
    */

//	unsigned long m_ulSSize;//�����δ�С
//	unsigned long m_ulDataSegSize;//���ݶδ�С
//	unsigned long m_ulVMemSize;//�����ڴ��С

	//�ⲿ�ɷ��ʵ�ַ�ռ�
	EXTERNALSPACE *m_pExternalSpaceList;
	long m_lExternalSpaceNum;
	long m_lExternalSpaceListSize;

	//���³�Ա�������ڵ���
//����ģʽ
#define VM_MODE_NORMAL 0
#define VM_MODE_DEBUG  1
#define VM_MODE_STEPDEBUG 2

	long m_nWorkMode;           //����ģʽ
//	FILE* m_debugOut;			//������������
	long m_nRemainCmdNum;       //����ִ�е�ָ����

	
//	void (*m_pInputFn)(long &lCmdID, long& p1, long& p2, char* p3);//���뺯��
//	long* m_BreakPoint;         //�ϵ��б�
//	long m_lBreakPtNum;         //�ϵ����

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

	//����ⲿ�ɷ��ʵ�ַ�ռ�
	void AddExternalSpace(unsigned char* start, unsigned long size);

	//�������
	long AttachParam(BYTE* pParam, int size_t);
	BOOL Run();
	void LoadFunction(CFunction* pFunc);
	void Reset();

	// for class$object loading
	CClassTable m_classTable;  // loaded class instance
	CObjTable m_objTable;		// loaded Objects
	CObjectInst*  LoadObject(CClassDes* c);

	//���³�Ա���ڵ���
	void SetWorkMode(int mode);
	void SetDbgCmd(long lCmdID, long p1, long p2, char* p3);
//	void SetDbgOut(FILE* dbgout){if (dbgout) m_debugOut = dbgout;};
	BOOL OutputMemToFile(char* szFileName);
	void OutputMemToFile(FILE* file);
	void DumpToFile();
	BOOL IsDebugBreak(){return m_bIsDebugBreak;};

	
private:

	void _LoadFunc(CFunction *pFunc);
	
	//���ú�ɾ���ϵ�
	BOOL SetBreakPoint(long line);
	BOOL DelBreakPoint(long line);
	
	//��֤��ַ�Ƿ��ڿ��õ�ַ��Χ��
	//���õ�ַ�ռ�: ��������� �������ڴ�, �Ĵ���, �������ݿ���ⲿ���õ�ַ�ռ�
	BOOL ValidateAddress(unsigned char* address);
	//Ԥ����˫�ֽ�ָ��
	BOOL Preprocess2(PCOMMAND cmd, int &op1mode, int &op2mode, int &op1reflvl, int &op2reflvl, unsigned char * &dest, unsigned char* &src);//
	//Ԥ�����ֽ�ָ��
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
