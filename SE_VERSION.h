#ifndef __SE_VERSION_H__
#define __SE_VERSION_H__

// version info
#define SE_BUILDDATE __DATE__
#define SE_BUILDTIME __TIME__

//////////////////////////////////////////////////////////////
// #define SE_VERSION _T("2.0.0 0010B")
// ------------------------------------------
// 这个版本包含以下修改
// 1. CVirtualMachine::_endcallpub函数在调用了外部公用函数后
// catch(...){throw;},这样可以让SE记录发生exceptioin的脚本函
// 数和行数，同时又可以用windbg的dump文件查找具体出错地点。
//
// 2. CVirtualMachine::run()在根据debug命做相应处理时，
// CVirtualMachine::m_DgbCmdLock没有出锁。
// 
// 3. 添加了获取当前调试的虚拟机的寄存器和内存内容的接口
// 4. 添加了启动时的versioin infomation log
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// #define SE_VERSION _T("2.0.0 0011B")
// ------------------------------------------
// 这个版本包含以下修改
// 1. 编译时遇到popdigit失败时没有做出错处理， 下面的代码引用
// dt1时发生accesse violation。 修改： 在所有popdigit失败的地方
// 记录出错信息并退出。
// 2. 在pushdigit时校验数据类型， 以便当代码某处没有给dttype赋值
// 时发现bug。
// 3. 修改数据类型定义， first type为0, 0 是dummytype， 就是没有
// 实际意义的数据类型， 是用来填充操作数栈的。
// 4. NeedCast函数在判断dt1的数据类型时，如果是dummytype， 就返回－1
// 表示不需要修改数据类型
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// #define SE_VERSION _T("2.0.0 0012B")
// ------------------------------------------
// 这个版本包含以下修改
// 1. 修改了CVirtualMachine::OutputMemroyToFile函数， 添加了
// CVirtualMachine::DumpToFile函数。当脚本发生Exception时dump
// 虚拟机的内存到文件。
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// #define SE_VERSION _T("2.0.0 0013B")
// ------------------------------------------
// 这个版本包含以下修改
// 1. CVirtualMachine添加了一个变量m_bDebugBreak表示调试状态VM
// 是否正在执行还是已经停下来等待调试命令的输入. 
// 2. 添加了接口IsDebugBreak来获取VM的调试状态
// 3. 添加了jni的接口(SE_IsDebugBreak)来获取当前调试的VM的调试状态。
// 4. OutputMemToFile中输出到文件的内容增加了当前执行的行数和寄存器值
// 5. 发现bug， java_interface(SE_1GetPFunctionTable)中调用CScriptFuncTable::GetFunction()
// 后没有用ReleaseFunc, 导致表被死锁。已修改。
// 6. 发现bug， compiler在分析函数调用时，如果没有参数，就不会调用ActualParams(v),
// 因此不会校验参数个数, 修改后， 在FunctionCall(v)中校验参数个数.
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// #define SE_VERSION _T("2.0.0 0014B")
// ------------------------------------------
// 这个版本包含以下修改
// 1. OutputMemToFile中输出到文件的内容修改了
// 2. 修改bug: 当catch到CVMException时输出log的代码有误.
// 3. 修改了断点的数据结构，用一个结构存放断点信息， 断点信息
// 包括函数名和IP值。

//////////////////////////////////////////////////////////////
// #define SE_VERSION _T("2.0.0 0015B")
// ------------------------------------------
// 1. 去掉了对"windows.h"的include
// 2. 添加了CVMManager. 
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
#define SE_VERSION "2.0.0 0016B"
// ------------------------------------------
// 1. modify CVirtualMachine::Reset(), make it more safe
// 2. 添加了se_interface, place all interface definition in the file
// 3. 修改了Java调用接口中取字符串的方式，解决了传递中文字符串的问题。
// 4. 纠正了SE_REQ_SetFnID函数的bug: 把hReq作为参数传给了SetFuncID
// 5. 纠正了SE_REQ_EXECScript的错误: 生成参数块时没有把原来的hSession去掉.
// 6. 修改了CPubFunction::AddFunction, 原来的代码似乎不安全
// 7. 1668~1680@cp.cpp有错, 把dt写成了td1, 而且dt没有初始化
// 导致如果if()中有浮点数的关系运算的话不能编译通过.
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
// 5.修改CVMManager::ExecScript, 使其支持在当前线程中运行脚本。
// 添加CVMManager::PrePareVM，负责分析请求和获取虚拟机。
// 添加CVMManager::UnPrepareVM， 负责运行完脚本后的处理。
// 删除SE_HandleRequest
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

