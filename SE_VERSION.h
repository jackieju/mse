#ifndef __SE_VERSION_H__
#define __SE_VERSION_H__

// version info
#define SE_BUILDDATE __DATE__
#define SE_BUILDTIME __TIME__

//////////////////////////////////////////////////////////////
// #define SE_VERSION _T("2.0.0 0010B")
// ------------------------------------------
// ����汾���������޸�
// 1. CVirtualMachine::_endcallpub�����ڵ������ⲿ���ú�����
// catch(...){throw;},����������SE��¼����exceptioin�Ľű���
// ����������ͬʱ�ֿ�����windbg��dump�ļ����Ҿ������ص㡣
//
// 2. CVirtualMachine::run()�ڸ���debug������Ӧ����ʱ��
// CVirtualMachine::m_DgbCmdLockû�г�����
// 
// 3. ����˻�ȡ��ǰ���Ե�������ļĴ������ڴ����ݵĽӿ�
// 4. ���������ʱ��versioin infomation log
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// #define SE_VERSION _T("2.0.0 0011B")
// ------------------------------------------
// ����汾���������޸�
// 1. ����ʱ����popdigitʧ��ʱû���������� ����Ĵ�������
// dt1ʱ����accesse violation�� �޸ģ� ������popdigitʧ�ܵĵط�
// ��¼������Ϣ���˳���
// 2. ��pushdigitʱУ���������ͣ� �Ա㵱����ĳ��û�и�dttype��ֵ
// ʱ����bug��
// 3. �޸��������Ͷ��壬 first typeΪ0, 0 ��dummytype�� ����û��
// ʵ��������������ͣ� ��������������ջ�ġ�
// 4. NeedCast�������ж�dt1����������ʱ�������dummytype�� �ͷ��أ�1
// ��ʾ����Ҫ�޸���������
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// #define SE_VERSION _T("2.0.0 0012B")
// ------------------------------------------
// ����汾���������޸�
// 1. �޸���CVirtualMachine::OutputMemroyToFile������ �����
// CVirtualMachine::DumpToFile���������ű�����Exceptionʱdump
// ��������ڴ浽�ļ���
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// #define SE_VERSION _T("2.0.0 0013B")
// ------------------------------------------
// ����汾���������޸�
// 1. CVirtualMachine�����һ������m_bDebugBreak��ʾ����״̬VM
// �Ƿ�����ִ�л����Ѿ�ͣ�����ȴ��������������. 
// 2. ����˽ӿ�IsDebugBreak����ȡVM�ĵ���״̬
// 3. �����jni�Ľӿ�(SE_IsDebugBreak)����ȡ��ǰ���Ե�VM�ĵ���״̬��
// 4. OutputMemToFile��������ļ������������˵�ǰִ�е������ͼĴ���ֵ
// 5. ����bug�� java_interface(SE_1GetPFunctionTable)�е���CScriptFuncTable::GetFunction()
// ��û����ReleaseFunc, ���±����������޸ġ�
// 6. ����bug�� compiler�ڷ�����������ʱ�����û�в������Ͳ������ActualParams(v),
// ��˲���У���������, �޸ĺ� ��FunctionCall(v)��У���������.
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// #define SE_VERSION _T("2.0.0 0014B")
// ------------------------------------------
// ����汾���������޸�
// 1. OutputMemToFile��������ļ��������޸���
// 2. �޸�bug: ��catch��CVMExceptionʱ���log�Ĵ�������.
// 3. �޸��˶ϵ�����ݽṹ����һ���ṹ��Ŷϵ���Ϣ�� �ϵ���Ϣ
// ������������IPֵ��

//////////////////////////////////////////////////////////////
// #define SE_VERSION _T("2.0.0 0015B")
// ------------------------------------------
// 1. ȥ���˶�"windows.h"��include
// 2. �����CVMManager. 
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
#define SE_VERSION "2.0.0 0016B"
// ------------------------------------------
// 1. modify CVirtualMachine::Reset(), make it more safe
// 2. �����se_interface, place all interface definition in the file
// 3. �޸���Java���ýӿ���ȡ�ַ����ķ�ʽ������˴��������ַ��������⡣
// 4. ������SE_REQ_SetFnID������bug: ��hReq��Ϊ����������SetFuncID
// 5. ������SE_REQ_EXECScript�Ĵ���: ���ɲ�����ʱû�а�ԭ����hSessionȥ��.
// 6. �޸���CPubFunction::AddFunction, ԭ���Ĵ����ƺ�����ȫ
// 7. 1668~1680@cp.cpp�д�, ��dtд����td1, ����dtû�г�ʼ��
// �������if()���и������Ĺ�ϵ����Ļ����ܱ���ͨ��.
//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////
// #define SE_VERSION _T("2.0.0 0016B")
// ------------------------------------------
// 1. fixed a bug in function "OutputMemToFile" which cause crash
// 2. add a interface to CCMManager "GetActiveVMNum" and the corresponding
// interface to dll
//
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
 #define SE_VERSION "2.0.0 0017B"
// ------------------------------------------
// 1. Modify the CVMManager class: Add the constraint of vm number
// 2. modify the CVMManager class: modify the ReleaseVM(), StopAllVM()
// 3. modify the CVirtualMachine: use event to control the vm runng states
// 4. modify the SE_GetVersion Method
//////////////////////////////////////////////////////////////

 #define SE_VERSION "2.0.0 0018B"
// ------------------------------------------
// 1. fix a bug in GetFreeVM(). original code use the reference between 
// two vector.
// 2. change type of CVMManager::m_ActiveVMList from vector to list, because
// it will use erase(), which is a lag operation in vector. Due to this, ReleaseVM()
// was modified.
//////////////////////////////////////////////////////////////

#define SE_VERSION "2.0.0 0019B"
// ------------------------------------------
// 1. (2003-03-04)
// Add another param "long lFlag" to function ExecScript(), so that
// CVMManager can provide tow way to run a script: in the same thread 
// or in another thread
// 2. (2003-02-28)
// add variable infomation to compilatioin output. 
// modify by Zhang Yuan
// changed files:
// cp.cpp;cs.cpp;CR_ERROR.h;CR_ERROR.cpp;CR_ABS.h
// 3. (2003-06-12)
// move logic from cp.cpp to ncpp.atg
//////////////////////////////////////////////////////////////


#define SE_VERSION "2.0.0 0020B"
// ------------------------------------------
// 1. modify conctructor of CVirtualMachine, create m_Stopped with
// initial status false. If initial status is true, the HandleRequest
// thread will hang up on the CVirtualMachine::Reset() call in case 
// it before run(). 
//
// 2. (2003-06-20)
// Transplant SE to AIX on IBM RS60000.
// Every thing is smooth but the assembly language. the PowerPC assembly system is so defferent from
// the x86 family. And article online about AIX assembly languag is lacking. xlC and dbx is not very
// friendly. And xlC dese not support the inline assembly, so I write the .s file and compile it to .o
// using as, then link it in makefile
// 
// 3.(2003-07-12)
// Fix a bug on IBM AIX. the LoadRequest and GenerateReq of Request.cpp use
// the actual number of bytes but not the sizeof(). so on AIX, the request package
// can not be parsed and generated correctly.
//
// 4. (2003-07-14)
// Fix another bug on IBM aix.
// change the checkpoint 2 value from 0xfff to -1. 
// I thought that the value 0xffff is not equal to -1 on aix, because the short is 4 bytes long
//////////////////////////////////////////////////////////////

// #define SE_VERSION "2.1.0 0000B"
// ------------------------------------------
// 1. move SE_HandleRequest interface to service module.
// 2. delete SE_ExecScript() interface and add tow interfaces:
// SE_RunScript()
// SE_ExecScript()
// 3. chang the return type of CVirtualMachine::LoadFunction() and CVirtualMachine::_LoadFunc and AddExternalSpace from BOOL to void, and change the return type of CVirtualMachine::AttachParam() from BOOL to long
// 4. add an interface "SE_AddPubFunc"
// 5.�޸�CVMManager::ExecScript, ʹ��֧���ڵ�ǰ�߳������нű���
// ���CVMManager::PrePareVM�������������ͻ�ȡ�������
// ���CVMManager::UnPrepareVM�� ����������ű���Ĵ���
// ɾ��SE_HandleRequest
// 6.   modify AddFunction()


//////////////////////////////////////////////////////////////
#define SE_VERSION "2.1.0 0001B"
// ------------------------------------------
//  1. modify the AttachParam()  when check size, the size_t must be equel to function specification
//  2. 	modiy the ActualParameters() function:
//	remove the code for checking parameter number of function call
//	add check for parameter at the end of the function

//  --------
//  2003-07-30
//  create a function SELog() to printf log , if the pLog param passed into SE_Start()
//  is NULL, the SE will use SELog as g_pLogFunc 
//////////////////////////////////////////////////////////////

#endif //__SE_VERSION_H__

