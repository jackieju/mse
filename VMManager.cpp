#include "clib.h"
#include "thread.h"

/************************************
  REVISION LOG ENTRY
  Revision By: Weihua Ju
  Revised on 2003-5-26 19:45:14
  Comments: 
  Add code in constructor:
  if (m_ActiveVMListLock.Create() != LOCKEX_ERR_OK)
		throw new CVMMException("create list lock failed");
 ************************************/


/************************************
  REVISION LOG ENTRY
  Revision By: Weihua Ju
  Revised on 2003-5-8 15:06:19
  Comments: 
  CVMManager::UnPrepareVM中没有delete request
  compiler的CVMManager有泄漏, 析构时没有删除vm_pool中的vm
 ************************************/


/************************************
  REVISION LOG ENTRY
  Revision By: Weihua Ju
  Revised on 2003-4-21 12:24:39
  Comments: 
  In ExecScript(): add try catch block to call of RunScriptThread

 ************************************/

#include "os/CSS_LOCKEX.H"
#include <list>
#include <stack>
#include <valarray>
//#include <vector>
#include "utility.h"
#include "Function.h"
#include "ScriptFuncTable.h"
#include "PubFuncTable.h"
#include "Function.h"
//#include "Request.h"
#include "VirtualMachine.h"
#include "Exp.h"
//#include "DOCommon.h"
#include "se_interface.h"
#include "compiler.h"
#include "VMManager.h"
#include "Request.h"


//extern CScriptFuncTable functable[SCRIPTTABLE_NUM];
//extern long g_lActiveScriptTable;

CVMManager::CVMManager()
{
	m_lVMInc = 10;
	m_lMaxVMNum = 100;
	
	if (m_StopEvent.Create(NULL, true, false, NULL) != LOCKEX_ERR_OK)
		throw new CVMMException("create stop event failed");
	if (m_VMPoolLock.Create() != LOCKEX_ERR_OK)
		throw new CVMMException("create list lock failed");
	if (m_IdleVMListLock.Create() != LOCKEX_ERR_OK)
		throw new CVMMException("create list lock failed");
	if (m_ActiveVMListLock.Create() != LOCKEX_ERR_OK)
		throw new CVMMException("create list lock failed");
};

#if 0
/*void	CVMManager::CreateFreeVM()	
{
	CVirtualMachine* pNew = NULL;
	pNew = new CVirtualMachine;
	if (pNew == NULL)
		return NULL;
	m_IVMListLock.WaitToWrite();
	m_IdleVMList.push_back(pNew);
	m_IVMListLock.DoneWriting();
	return ;
};
*/


/**
Declaration		: CVirtualMachine* CVMManager::GetFreeVM()
Decription		: get available vm, after the vm be got, it will neither in the running list
					or in the idle list
Parameter		: 
Return value	: CVirtualMachine*  - the vm be got
Written by		: Weihua Ju
Completed at	: 2002-7-3
**/
CVirtualMachine* CVMManager::GetFreeVM()
{
	CVirtualMachine* pNew = NULL;
	BOOL bGot;

	m_IVMListLock.WaitToRead();
	if (m_IdleVMList.size() <= 0)
		bGot = FALSE;
	else
		bGot = TRUE;
	m_IVMListLock.DoneReading();

	if (bGot = FALSE)
	{
		pNew = new CVirtualMachine;
		if (pNew == NULL)
		{
			throw new CVMMException("allocate memory failed");
			return NULL;
		}
		else
			return pNew;
	}
	else
	{
		m_IVMListLock.WaitToWrite();
		pNew = m_IdleVMList.back();
		m_IdleVMList.pop_back();
		m_IVMListLock.DoneWriting();
		return pNew;
	}
}
#endif

void CVMManager::UnPrepareVM(RUNSRCIPT_STR* pStruct)
{

	if (pStruct->pReadyParam)
	{
		delete pStruct->pReadyParam;
		pStruct->pReadyParam = NULL;
	}

	char** str = NULL;
	std::list<char**>::iterator it;
	for (it = pStruct->arStr.begin(); it != pStruct->arStr.end(); it++)
	{
		str = *it;
		if (str)
			delete str;
		*it = NULL;
	}
	pStruct->arStr.clear();

	if (pStruct->pfn!= NULL)
		pStruct->pFunctionTable->ReleaseFunc();
	

	if (pStruct->machine)
	{
		ReleaseVM(pStruct->lVMIndex);
		pStruct->machine = NULL;
	}

	SAFEDELETE(pStruct->request);

	
}

long CVMManager::PrePareVM(RUNSRCIPT_STR* pStruct)
{
	CVMManager* pVMMger = pStruct->pVMger;
	long		lWorkMode = pStruct->lWorkMode;

	char szMsg[301] = "";
//	std::list<char**>	arStr;	// 保存new的字符串指针数组
//	long lStrNum = 0;

	int i, j;
	long index = 0;
	REQPARAM* pParamList = NULL;	// 请求参数链表
	int   paramsize = 0;			//参数块的最后长度
	BYTE* pBlock = NULL;			// parameter block passed to vm
	char** strArray = NULL;
	long lParamHeaderSize = sizeof(PARAMHDR);
	char* szFuncID = NULL;
	long ret = REQERR_NOERROR;


	//find function by name
	szFuncID = pStruct->request->GetFuncID();
	pStruct->pFunctionTable = &(CCompiler::functable[CCompiler::g_lActiveScriptTable]);
	pStruct->pfn = pStruct->pFunctionTable->GetFunction(szFuncID, &index);
	if (pStruct->pfn == NULL || index < 0)
	{
		snprintf(szMsg, 300, "SE:: find script function \"%s\"failed", szFuncID);
		nLOG(szMsg, 2);
		return REQERR_FUNCNOTFOUND;
	}
	snprintf(szMsg, 300,"SE:: find script function \"%s\" success", szFuncID);
	nLOG(szMsg, 200);

	// get free VM
	pStruct->machine = pVMMger->GetFreeVM(pStruct->lVMIndex);
	if (pStruct->machine == NULL)
	{
		nLOG("SE:: get free VM failed", 2);
		return SEERR_GETFREEVMFAILED;
	}

	//load function
	pStruct->machine->LoadFunction(pStruct->pfn);
	
	nLOG("SE:: Load Function OK.", 200);
		
	// set work mode
	pStruct->machine->SetWorkMode(lWorkMode);
	snprintf(szMsg, 300, "SE:: Set work mode to %d", lWorkMode);
	nLOG(szMsg, 100);
	
	// prepare parameters
	if (pStruct->request->GetParamNum() > 0)
	{
		pParamList = pStruct->request->GetParamList();
		if (pParamList == NULL)	
		{
			nLOG("SE:: get request parameter list failed", 2);
			throw new CVMMException("Get request parameter list failed");
			//	return false;
		}
		
		//compute size of all param
		for (i = 0; i < pStruct->request->GetParamNum(); i++)
		{
			if (pParamList[i].dataType == dtString 
#ifdef __SUPPORT_OBJ
				|| pParamList[i].dataType == dtObject 
#endif
				||
				pParamList[i].arraydim > 0)
			{
				pStruct->machine->AddExternalSpace((unsigned char*)pParamList[i].content, pParamList[i].contentSize);
			}
			if (pParamList[i].arraydim > 0)
				//if it is array, user point
			{
				paramsize += sizeof(long*);
			}
			else
			{
				if (pParamList[i].dataType == dtString 
#ifdef __SUPPORT_OBJ
					|| pParamList[i].dataType == dtObject
#endif
					)
					//it is not array but it is string or object, use point
				{
					paramsize += sizeof(long*);
				}
				else
				{
					paramsize += pParamList[i].contentSize;
				}
			}
		}	
		nLOG("SE:: Analyze param success", 200);
			
		//create ready param block
		pStruct->pReadyParam = new BYTE[paramsize];
		if (pStruct->pReadyParam == NULL)
		{
			nLOG("SE:: Allocate memory for parameter block failed", 2);
			throw new CVMMException("Allocate memory for parameter block failed");
			//	return;
		}
		pBlock = pStruct->pReadyParam;
		
		nLOG("SE:: Create parameter block success", 200);
			
			
		/*	//generate param content
		if (pBlock + sizeof(void*) - pReadyParam > paramsize)
		{
		nLOG("SE:: parameter block overflow.", 2)
		goto CleanUp;
		}
		pBlock += sizeof(void*);
		*/
		
		for (i = 0; i < pStruct->request->GetParamNum(); i++)
		{
			char* point;
			point = (char*)pParamList[i].content;
			if (pParamList[i].arraydim > 0)
				//if it is array, use point
			{
				if (pParamList[i].dataType == dtString)
				{//if it is string , generate string point array
					strArray = new char*[pParamList[i].arraysize[0]];				
					if (strArray == NULL)
					{
						nLOG("SE:: Allocate memory for sting array failed", 2);
						throw new CVMMException("SE:: Allocate memory for sting array failed");
						//	return;
					}
					//	arStr[lStrNum] = strArray;
					//	lStrNum++;
					pStruct->arStr.push_back(strArray);
					pStruct->machine->AddExternalSpace((unsigned char*)strArray, pParamList[i].arraysize[0]* sizeof(char*));
						
						for (j = 0; j< pParamList[i].arraysize[0]; j++)
						{
							strArray[j] = (char*)point;
							point += strlen((const char *)point)+1;
						}
						if (pBlock + sizeof(long*) - pStruct->pReadyParam > paramsize)
						{
							nLOG("SE:: parameter block overflow.", 2);
							throw new CVMMException("SE:: parameter block overflow");					
							//return;
						}				
						memcpy(pBlock, (void*)&strArray, sizeof(long*));
						pBlock += sizeof(long);
				}
				else
				{//if it is not string, copy
					if (pBlock + sizeof(long*) - pStruct->pReadyParam > paramsize)
					{
						nLOG("SE::  parameter block overflow.", 2);
						throw new CVMMException("SE:: parameter block overflow");					
						//	return;
					}				
					memcpy(pBlock, (void*)&pParamList[i].content, sizeof(long*));
					pBlock += sizeof(long*);
				}
			}
			else
			{//if it is simple param, copy content
				if (pParamList[i].dataType == dtString)
					//if it is string , user point
				{
					if (pBlock + sizeof(long*) - pStruct->pReadyParam > paramsize)
					{
						nLOG("SE::  parameter block overflow.", 2);
							throw new CVMMException("SE:: parameter block overflow");
						//return;
					}				
					memcpy(pBlock, (void*)&pParamList[i].content, sizeof(long*));
					pBlock += sizeof(long);
				}
#ifdef __SUPPORT_OBJ
				else if (pParamList[i].dataType == dtObject)
				{
					if (pBlock + sizeof(long*) - pStruct->pReadyParam > paramsize)
					{
						nLOG("SE::  parameter block overflow.", 2);
						throw new CVMMException("SE:: parameter block overflow");
						//return;
					}
					memcpy(pBlock, (void*)&pParamList[i].content, sizeof(long*));
					pBlock += sizeof(long);
				}
#endif
				else
				{
					if (pBlock + pParamList[i].contentSize - pStruct->pReadyParam > paramsize)
					{
						nLOG("SE::  parameter block overflow.", 2);
						throw new CVMMException("SE:: parameter block overflow");
						//return;
					}
					memcpy(pBlock, (void*)pParamList[i].content, pParamList[i].contentSize);
					pBlock += pParamList[i].contentSize;
				}
			}
		}
		nLOG("SE:: Generate param contest success", 200);
	}


	//attach param
	ret = pStruct->machine->AttachParam(pStruct->pReadyParam, paramsize);
	if (ret != SEERR_NOERROR)
	{
		nLOG("SE:: Attach parameter failed.", 9);
		return ret;
	}
	
	snprintf(szMsg, 300,"SE:: VMachine run(function:\"%s\", workmode:%d, param block size:%d, VM index:%d", szFuncID, lWorkMode, paramsize, pStruct->lVMIndex);
	nLOG(szMsg, 90);

	return REQERR_NOERROR;

}



/**
Declaration		: void CVMManager::RunScriptThread(void* p)
Decription		: run one script
Parameter		: 
			[IN]void* p	-	RUNSRCIPT_STR struct point, this funciont will delete it including the request object in it
Return value	: void  - 
Written by		: Weihua Ju
Completed at	: 2002-9-3
*/
void CVMManager::RunScriptThread(void* p)
{
	char szMsg[301] = "";
	RUNSRCIPT_STR* pStruct = (RUNSRCIPT_STR*)p;

	//------------ call function ------------//


	try{
		pStruct->machine->Run();
	}
	catch (...) {
		// exit safely beforce throwing
		pStruct->pVMger->UnPrepareVM(pStruct);
		SAFEDELETE(pStruct);

		throw;
	}



	//------------ call function end------------//

	snprintf(szMsg, 300, "SE:: VMachine stopped(function:\"%s\", workmode:%d.", pStruct->sFuncName.c_str(), pStruct->lWorkMode);
	nLOG(szMsg, 90);

	
	pStruct->pVMger->UnPrepareVM(pStruct);
	SAFEDELETE(pStruct);

	return ;
}
	
void CVMManager::GetAllActiveVMInfo(std::vector<VMINFO> *info)
{
		if (!info)
			return;

		m_ActiveVMListLock.WaitToRead();

		// code modified by ju, 2003-01-22
//		int i = 0;
//		for (i = 0;i < m_ActiveVMList.size(); i++)
//		{
//			VMINFO tempInfo;
//			long index = m_ActiveVMList[i];
//			CVirtualMachine* vm = m_VMPool[index];
//			tempInfo.id = index;
//			vm->GetStatus(&tempInfo.status);
//			vm->GetStartTime(&tempInfo.start_time);
//			info->push_back(tempInfo);
//		}
		
		std::list<long>::iterator it;
		for (it = m_ActiveVMList.begin(); it != m_ActiveVMList.end(); ++it)
		{
			VMINFO tempInfo;
			long index = *it;
			CVirtualMachine* vm = m_VMPool[index];
			tempInfo.id = index;
			vm->GetStatus(&tempInfo.status);
			vm->GetStartTime(&tempInfo.start_time);
			info->push_back(tempInfo);
		}
			
		m_ActiveVMListLock.DoneReading();
}

void CVMManager::GetVMInfo(VMSTATUS* status, long lIndex)
{
	if (status == NULL || lIndex < 0)
		return;
	if (lIndex >= m_VMPool.size() )
		return;
	CVirtualMachine* vm = m_VMPool[lIndex];
	vm->GetStatus(status);
	return;
}



/**
	Declaration		: long ExecScript(CRequest* request, long lWorkMode)
	Decription		: execute script
	Parameter		: 
				[IN]CRequest* request	-	reqeust
				[IN]long lWorkMode	-	work mode , 0: normal 1: debug
				[IN]long lFlag		-	0: in the same thread, 1: launch another thread
				Return value	: void  - 
				Written by		: Weihua Ju
				Completed at	: 2002-7-4
**/
long CVMManager::ExecScript(CRequest* request, long lWorkMode, long lFlag)
{
	long ret = REQERR_NOERROR;
	if (request == NULL)
		throw new CVMMException("request pointer is null");
	
	if (m_StopEvent.Wait(0) != LOCKEX_ERR_TIMEOUT)
		return SEERR_INSTOPPING;
	
	
	RUNSRCIPT_STR* str= new RUNSRCIPT_STR;
	if (str == NULL)
		throw new CVMMException("new RUNSRCIPT_STR failed");
	
	str->lWorkMode = lWorkMode;
	str->pVMger = this;
	str->request = new CRequest(*request);	// copy from original request	
	if (str->request == NULL)
		throw new CVMMException("new CRequest from source Reqeuest failed");
	
	ret = PrePareVM(str);
	if (ret != SEERR_NOERROR)
	{
		return ret;
	}
	
	switch(lFlag) 
	{
		
	case 0:
		try
		{
		
		RunScriptThread(str);
		}
		catch(...)
		{
			// exit safely before throwing
			throw;
		}
		//UnPrepareVM(str);
//		if (str)
//		{
//			delete str;
//			str = NULL;
//		}
		return REQERR_NOERROR;		
		break;
		
	case 1:
		if (JUJU::thread_create_real(RunScriptThread, str, "") < 0)
		{
			if (str)
			{
				delete str;
				str = NULL;
			}
			
			throw new CVMMException("create thread failed");
		}
		break;
		
	default:
		if (str)
		{
			delete str;
			str = NULL;
		}
		throw new CVMMException("invalid flag");
		
	}
	
	
	return REQERR_NOERROR;
}
