/************************************
  REVISION LOG ENTRY
  Revision By: Weihua Ju
  Revised on 2003-5-23 12:27:06
  Comments: 
  fix a bug a ReleaseVM() when erase iterator
 ************************************/

/************************************
  REVISION LOG ENTRY
  Revision By: Weihua Ju
  Revised on 2003-3-4 14:18:42
  Comments: 
  1. Add another param "long lFlag" to function ExecScript(), so that
  CVMManager can provide tow way to run a script: in the same thread 
  or in another thread
  
 ************************************/



#ifndef __VMMANAGER_H__
#define __VMMANAGER_H__

#include <list>

// define error code

// end of error definition
//typedef std::vector<CVirtualMachine*> vm_list; // virtual machine process list
typedef std::deque<long>	int_list;
typedef std::valarray<CVirtualMachine*> vm_pool; // virtual machine process array

class CRequest;
struct VMMStatus
{	
	long lRunningVMNum;		// vm thoese are in runing status;
	long lIdleVMNum;		// vm those are in idle status;
	long lTotleVMNum;		// totle vm number
};
struct VMINFO;

//struct VMINFO
//{
//	SYSTEMTIME start_time;
//	VMSTATUS	status;
//};

class CVMMException
{
public:
	CVMMException(char* msg){m_sMsg = msg;};
	virtual ~CVMMException(){
	};

	void Report(std::string& msg){msg = m_sMsg;};
	const char* Report(){return m_sMsg.c_str();};
private:
	std::string	m_sMsg;	
};

class CVMManager;
struct RUNSRCIPT_STR
{
	CVMManager* pVMger;
	CRequest*	request;
	std::string sFuncName;
	CVirtualMachine* machine;
	long		lVMIndex;
	unsigned char* pReadyParam;
	long		lWorkMode;
	std::list<char**>	arStr;
	CFunction	*pfn;
	CScriptFuncTable* pFunctionTable;	
	long		ret;
	RUNSRCIPT_STR()
	{
		pVMger = NULL;
		request = NULL;
		machine = NULL;
		lVMIndex = 0;
		pReadyParam = NULL;
		lWorkMode = 0;
		pfn = NULL;
		pFunctionTable = NULL;
		ret = 0;
	}
};


class CVMManager
{
public:
	CVMManager();
	virtual ~CVMManager(){
	//	CVirtualMachine* vm = NULL;
		int i;
		for (i = 0; i < m_VMPool.size(); i++)
			SAFEDELETE(m_VMPool[i]);
		m_StopEvent.Destroy();
		m_VMPoolLock.Destroy();
		m_IdleVMListLock.Destroy();	
	};
	long ExecScript(CRequest* request, long lWorkMode, long lFlag = 0);
		
		

	static void RunScriptThread(void* p);		// run one script using VM
	void StopAllVM()
	{
		ReleaseAllVM();
		m_StopEvent.Set();
	};

	void SetMaxVMNum(long number)
	{
		m_lMaxVMNum = number;
	};
	long GetActiveVMNumber()
	{
		long ret  = 0;
		m_ActiveVMListLock.WaitToRead();
		ret = m_ActiveVMList.size();	
		m_ActiveVMListLock.DoneReading();
		return ret;
	};
	
	void GetVMInfo(VMSTATUS* status, long lIndex);
	void GetAllActiveVMInfo(std::vector<VMINFO> *info);
private:
	long PrePareVM(RUNSRCIPT_STR* pStruct);
	void UnPrepareVM(RUNSRCIPT_STR* pStruct);
	void ReleaseAllVM()
	{
		int i = 0;
		m_VMPoolLock.WaitToWrite();
		for (i = 0; i< m_VMPool.size(); i++)
			ReleaseVM(i);		
		m_VMPoolLock.DoneWriting();
	}
//	void CreateFreeVM();		//  create new free vm and add to idle vm list

//	CVirtualMachine* GetFreeVM();						// get available vm
//	void ReleaseVM(CVirtualMachine* pVM);				// move vm from running list to idle list
	
	// remove vm from idle list and return the index of vm in vm pool
	CVirtualMachine* GetFreeVM(long &lIndex)
	{
		lIndex = -1;
		long rIndex;
		long lCount = 0;

		// while there are VM free, expand vm pool
		while (1)			
		{
			if (lCount >0)
				if (m_StopEvent.Wait(10) != LOCKEX_ERR_TIMEOUT)
					break;
			m_IdleVMListLock.WaitToWrite();
			if (m_IdleVMList.empty())
			{

				m_IdleVMListLock.DoneWriting();
				ExpandVMPool();	
				lCount++;
				continue;
			}
			else
				break;
		}

		rIndex = m_IdleVMList.front();
		lIndex = rIndex;
		m_IdleVMList.pop_front();
		m_IdleVMListLock.DoneWriting();

		m_ActiveVMListLock.WaitToWrite();
		m_ActiveVMList.push_back(rIndex);
		lIndex = m_ActiveVMList.back();
		m_ActiveVMListLock.DoneWriting();

		return m_VMPool[lIndex];
	};	

	// add vm to idle list
	void ReleaseVM(long lVMIndex)
	{
		m_VMPool[lVMIndex]->Reset();

		m_ActiveVMListLock.WaitToWrite();

		// code modified by ju, 2003-01-22
		// search
		std::list<long>::iterator it;
		it = m_ActiveVMList.begin();
		//long s = m_ActiveVMList.size();
		//int i =  0;
		//for (; i < s; i++)
		//{
		//	if (m_ActiveVMList[i] == lVMIndex)
		//		break;
		//}
		it = m_ActiveVMList.begin();
		while (it != m_ActiveVMList.end())
		{
			if (*it == lVMIndex)
			{
				m_ActiveVMList.erase(it);
				break;
			}
			it++;
		}
		//	if (i < s)
	//		m_ActiveVMList.erase(m_ActiveVMList.begin()+i);
		m_ActiveVMListLock.DoneWriting();
		
		m_IdleVMListLock.WaitToWrite();			
		m_IdleVMList.push_back(lVMIndex);
		m_IdleVMListLock.DoneWriting();

	};
	
	// expand vm pool using m_lVMInc
	void ExpandVMPool()
	{
		m_VMPoolLock.WaitToWrite();

		long l = m_VMPool.size();
		if (l >= m_lMaxVMNum)
		{
			m_VMPoolLock.DoneWriting();
			return;
		}

		// lengthen array
		long lInc = m_lVMInc;
		if (l + m_lVMInc > m_lMaxVMNum)
			lInc = m_lMaxVMNum - l;

		m_VMPool.resize(l + lInc, NULL);

		m_IdleVMListLock.WaitToWrite();
		// generate vm
		for (long i = l ; i < m_VMPool.size(); i++)
		{
			CVirtualMachine* pNew = new CVirtualMachine();
			if (pNew == NULL)
				throw new CVMMException("allocate memory failed");
			m_VMPool[i] = pNew;
			m_IdleVMList.push_back(i);
		}
		m_IdleVMListLock.DoneWriting();
		
		m_VMPoolLock.DoneWriting();
	};

private:
//	vp_list		m_RunningVMList;			// list of running vm
//	vm_list		m_IdleVMList;				// list of idle vm
//	CSS_SingleWriteMultiRead	m_RVMListLock;		// lock of running vm list
//	CSS_SingleWriteMultiRead	m_IVMListLock;		// lock of idle vm list
//	std::string	m_sOutFileName;				// path of output file	


	vm_pool		m_VMPool;		// vm pool
	int_list	m_IdleVMList;	// store the free vm index, get free vm from front and add free to back 
	// code modified by ju, 2003-01-22
	std::list<long>	m_ActiveVMList;	// store the running vm index
	CSS_SingleWriteMultiRead	m_VMPoolLock; // lock for vm pool
	CSS_SingleWriteMultiRead	m_IdleVMListLock;	// lcok for idle vm list
	CSS_SingleWriteMultiRead	m_ActiveVMListLock;	// lcok for idle vm list
	long		m_lVMInc;		//  increment of VM when expand vm pool
	CSS_EVENT	m_StopEvent;	// event to stop all vm
		long		m_lMaxVMNum;
};
#endif //__VMMANAGER_H__

