// LogCache.cpp: implementation of the CLogCache class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "osmacros.h"
#include "LogLevel.h"
#include "LogCache.h"
#include "ostime.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//	函数说明：	CLogCache();
//	函数功能：	构造函数
//	变量说明：	无
//	返回值：	无
//	编写人：	刘骏&张渊&陈学光
//	完成日期：
////////////////////////////////////////////////////////////////////////		
CLogCache::CLogCache()
{
	m_bCacheMode = true;
	m_bInit = false;
	m_nAttrib = 0;
	
	m_nSeparator = 0;
	m_nBufferSize = 0;
	m_nSaveMode = modeOneFile;
	m_nTail = 0;
	m_nMaxCacheNum = 0;
	m_nCount = 0;

	m_nFieldNum = 0;
	m_nBaseErrorLevel = 0;

	m_pBuffer = NULL;
	m_pTempBuffer = NULL;
	m_nRepeatNum = 0;
	m_pCacheListHead = m_pCacheListTail = NULL;

	m_BufMutex.Create(NULL);
	
//	m_sFileName[0] = 0;
	strcpy(m_sFileName, STATIC_LOG_FILE);

	m_sSysName[0] = 0;
	m_sVersion[0] = 0;
	m_sFieldName[0] = 0;

	m_nMaxFreeDisk = 10485760;
	
	m_hCriticalSection.Create(NULL);
	m_hCriticalSectionCacheList.Create(NULL);

	// allocate temp memory space
	m_sMsgTemp = new char[TEMPMSG_LEN];
}

////////////////////////////////////////////////////////////////////////
//	函数说明：	~CLogCache();
//	函数功能：	析构函数
//	变量说明：	无
//	返回值：	无
//	编写人：	刘骏&张渊&陈学光
//	完成日期：
////////////////////////////////////////////////////////////////////////		
CLogCache::~CLogCache()
{
	//  change by jaylv 2002.02.25  start
	//  在连续释放两次内存时防止错误
	if ( m_bInit == false )
		return;
	//  change by jaylv 2002.02.25  end

	Destroy();
	m_hCriticalSection.Destroy();
	m_hCriticalSectionCacheList.Destroy();

	//delete temporary space
	SAFEDELETE(m_sMsgTemp);
}

void CLogCache::Put(long level, char* file, long line, char* format, ...)
{
	long size1 = 0;
	long size2 = 0;	
	char* p = NULL;
	va_list arglist;
	
	if (level > m_nBaseErrorLevel)
		return;
	
	if (file == NULL)
		return;

	m_BufMutex.Lock();	// yangyan add 2002-08-15

	m_sMsgTemp[TEMPMSG_LEN-1] = 0;
	

	// write log level
	level = level > 100 ? 100 : level;
	level = level < 0 ? 0 : level;

	size1 = snprintf(m_sMsgTemp, TEMPMSG_LEN-1, "[%s] ", OSLOG_GetLogLevelName(level));
	if (size1 < 0)
	{
		m_BufMutex.Unlock();
		StaticPut(STATIC_LOG_FILE, "msg is too long, log msg truncateed.", __FILE__, __LINE__);
		return;
		
	}
	// write content
	p = m_sMsgTemp + size1;

    va_start(arglist, format);
	size2 = vsnprintf(p, TEMPMSG_LEN - size1 - 1, format, arglist);
    va_end(arglist);

	if (size2 < 0)
	{
		m_BufMutex.Unlock();
		StaticPut(STATIC_LOG_FILE, "msg is too long, log msg truncateed.", __FILE__, __LINE__);
		return;
	}
	
	// write file and line
	p = p + size2;
	snprintf(p, TEMPMSG_LEN - size1 - 2 - size2, " <%s, %d>", file, line);
	
	Put(m_sMsgTemp);
	
	m_BufMutex.Unlock();
	
	if (level < OS_LOG_ERROR)
		Flush();
	
}

/**
Declaration		: void CLogCache::StaticLog(char* filename, char* msg, char* sourcefile, long line, char* format)
Decription		: 
Parameter		: 
[IN]char* filename	-	log file name
[IN]char* msg	-	log msg
[IN]char* sourcefile	-	source file		
[IN]long line	-	source code line
[IN]char* format	-	log format for msg , source file and line
Return value	: void  - 
Written by		: 
Completed at	: 2002-8-1
*/
void CLogCache::StaticPut(char* filename, char* msg, char* sourcefile, long line, char* format)
{
	FILE* file = NULL;
	SYSTEMTIME time;
	// generate file name
	char fname[_MAX_PATH];

	if ( !msg || !sourcefile|| line < 0)
	{
//		Beep(1000, 500);	
		return;
	}

	GetLocalTime(&time);

	if (!filename || strlen(filename) == 0)
	{
		snprintf(fname, _MAX_PATH, "%04d-%02d-%02d", time.wYear, time.wMonth, time.wDay);
	}
	else
	{
		char temp[_MAX_PATH];

		strcpy(temp, filename);
		// get extension name
		char *p = strrchr(temp, '.');		

		if (p)
		{
			*p = '\0';
			snprintf(fname, _MAX_PATH, "%s%04d-%02d-%02d.%s", temp, time.wYear, time.wMonth, time.wDay, p+1);
		}
		else
			snprintf(fname, _MAX_PATH, "%s%04d-%02d-%02d", temp, time.wYear, time.wMonth, time.wDay);
	}

	file = fopen(fname, "a+");
	if (!file)
	{
//		Beep(1000, 500);
		return;
	}
	
	fprintf(file, "%04d-%02d-%02d %02d:%02d:%02d\t", time.wYear, time.wMonth, time.wDay, 
		time.wHour, time.wMinute, time.wSecond);

	if (format)
		fprintf(file, format, msg, sourcefile, line);
	else
		fprintf(file, "%s, file=%s, line=%d", msg, sourcefile, line);

	fprintf(file, "\n");	// add by yangyan 2002-08-07

	fclose(file);

	return;
}


////////////////////////////////////////////////////////////////////////
//	函数说明：	void Put(char *pMsg,long nErrorLevel = 0);
//	函数功能：	写入文本日志内容
//	变量说明：	pMsg            日志字符串
//				nErrorLevel     日志等级记录,如大于m_nBaseErrorLevel
//								则不写入日志
//	返回值：	无
//	编写人：	刘骏&张渊&陈学光
//	完成日期：
////////////////////////////////////////////////////////////////////////		
void CLogCache::Put(char *pMsg,long nErrorLevel)
{
	char sLogFileName[LOGCACHE_MAX_PATH] = {0};
	long tNow;
	FILE* pFile = NULL;
	bool bFileExist = false;
	long nLength;
	char strBuffer[256];
	struct tm *pTime;
	int i;
	bool bRepeat = false;

	m_hCriticalSectionCacheList.Lock();

//  如使用内存数据块节点，则需使用下一内存数据块节点
	while (m_pCacheListHead)
	{
		m_pCacheListTail = m_pCacheListHead->pNext;
		m_pCacheListHead = m_pCacheListTail;
		m_nCount++;
	}
//	超过最大缓存条数，写入文件
	if (m_nCount >= m_nMaxCacheNum)
	{
		FlushList();
		m_nCount = 0;
	}
	m_hCriticalSectionCacheList.Unlock();
//	check parameter
	if((pMsg == NULL)||(m_bInit == false))
		return;

//	check parameter
	switch(m_nAttrib)
	{
	case attLog:
		if(nErrorLevel > m_nBaseErrorLevel)
			return;
		break;
	case attDBLog:
		//	如为写入数据库结构型日志，判断写入内容与字段数是否一致
		if(m_nFieldNum != FindSeparatorNumber(pMsg)	+ 1)
			return;
		break;
	default:
		return;
		break;
	}
// generate timestamp
	tNow = time(NULL);
	pTime = localtime(&tNow);
//	生成标准头说明
	sprintf(strBuffer,
			"%04d/%02d/%02d%c%02d:%02d:%02d%c",
			pTime->tm_year + 1900,
			pTime->tm_mon + 1,
			pTime->tm_mday,
			m_nSeparator,
			pTime->tm_hour,
			pTime->tm_min,
			pTime->tm_sec,
			m_nSeparator);

// generate timestamp string
	GenerateFileName(sLogFileName,tNow);
	nLength = strlen(pMsg);
// 资源互斥
	m_hCriticalSection.Lock();
//   直接写入数据
	if(m_bCacheMode == false)
	{
	//	write log file directly
		pFile = fopen(sLogFileName,"r");
		if(pFile == NULL)
			bFileExist = false;
		else
		{
			bFileExist = true;
			fclose(pFile);
		}
		pFile = fopen(sLogFileName,"a");
		if(pFile == NULL)
		{
			m_hCriticalSection.Unlock();
			return;
		}
	//	if file no exist then create it and write head
		if(bFileExist == false)
			WriteFileHead(pFile);
	//	write message
		fwrite(strBuffer,sizeof(char),strlen(strBuffer),pFile);
		fwrite(pMsg,sizeof(char),nLength,pFile);
		fwrite(CRLF,sizeof(char),strlen(CRLF),pFile);
		fclose(pFile);
	}
	else
	{
		if(nLength + strlen(strBuffer) + strlen(CRLF) >= (unsigned long)m_nBufferSize)
		{
			FlushAll();
		//	write log file directly
			pFile = fopen(sLogFileName,"r");
			if(pFile == NULL)
				bFileExist = false;
			else
			{
				bFileExist = true;
				fclose(pFile);
			}
			pFile = fopen(sLogFileName,"a");
			if(pFile == NULL)
			{
				m_hCriticalSection.Unlock();
				return;
			}
		//	if file no exist then create it and write head
			if(bFileExist == false)
				WriteFileHead(pFile);
			fwrite(strBuffer,sizeof(char),strlen(strBuffer),pFile);
			fwrite(pMsg,sizeof(char),nLength,pFile);
			fwrite(CRLF,sizeof(char),strlen(CRLF),pFile);
			fclose(pFile);
			m_hCriticalSection.Unlock();
			return;
		}
	//  已无法缓存本条数据，需重新写入
		if ((m_nTail + nLength + strlen(strBuffer) + strlen(CRLF)) > (unsigned long)m_nBufferSize)
		{
		//	the buffer space remained is too small to contain this message,need to generate a new buffer
			CACHE_NODE *pCacheNode = new CACHE_NODE;
			if(pCacheNode == NULL)
			{
				m_hCriticalSection.Unlock();
				return;
			}
			pCacheNode->nTail = m_nTail;
			pCacheNode->pBuffer = m_pBuffer;
		// 加入缓冲区块列表
			PutCacheBlock(pCacheNode);
		// 生成新缓冲区块
			m_pBuffer = new char[m_nBufferSize + 2];
			if (!m_pBuffer)
			{
				m_pBuffer = pCacheNode->pBuffer;
				delete pCacheNode;
				m_hCriticalSection.Unlock();
				return;
			}
			memset(m_pBuffer,0x00,m_nBufferSize + 2);
			m_nTail = 0;
			m_nCount++;
		}
	//	Add By Chen XueGuang
		for (i = 0;i < 2*m_nRepeatNum; i++)
		{
			if (m_pTempBuffer[i][0] == 0)
			{
				strcpy(m_pTempBuffer[i],pMsg);	
				break;
			}
		}
		if (i == 2*m_nRepeatNum - 1)
		{
			bRepeat = false;
			for(i = 0;i < m_nRepeatNum;i++)
			{
				if (memcmp(m_pTempBuffer[i],m_pTempBuffer[m_nRepeatNum + i],strlen(m_pTempBuffer[i])) != 0)
				{
					bRepeat = true;
					break;
				}
			}
			if (bRepeat)
			{
				if ((long)(m_nTail + strlen(strBuffer)) <= m_nBufferSize)
				{
					strcpy(m_pBuffer + m_nTail,strBuffer);
					m_nTail += strlen(strBuffer);
				}
				if ((long)(m_nTail + strlen(m_pTempBuffer[0])) <= m_nBufferSize)
				{
					strcpy(m_pBuffer + m_nTail,m_pTempBuffer[0]);
					m_nTail += strlen(m_pTempBuffer[0]);
				}
				if ((long)(m_nTail + strlen(CRLF)) <= m_nBufferSize)
				{
					strcpy(m_pBuffer + m_nTail,CRLF);
					m_nTail += strlen(CRLF);
				}
				for(i = 0;i < 2*m_nRepeatNum - 1;i++)
				{
					strcpy(m_pTempBuffer[i],m_pTempBuffer[i+1]);
				}
				memset(m_pTempBuffer[2*m_nRepeatNum - 1],0x00,sizeof(char)*m_nBufferSize);
			}
			else
			{
				for(i = 0;i < m_nRepeatNum;i++)
				{
					memset(m_pTempBuffer[m_nRepeatNum+i],0x00,sizeof(char)*m_nBufferSize);
				}
			}
		}
	}
	// 离开临界区
	m_hCriticalSection.Unlock();
}

////////////////////////////////////////////////////////////////////////
//	函数说明Put(char *pMsg,char *pcLogFileName,long lLogPosition,long nErrorLevel);
//	函数功能：	写入文本日志内容，增加行号选项
//	变量说明：	pMsg            日志字符串
//				pcLogFileName   产生日志的源文件名
//				lLogPosition	产生日志的行号
//				nErrorLevel     日志等级记录,如大于m_nBaseErrorLevel
//								则不写入日志
//	返回值：	无
//	编写人：	刘骏&张渊&陈学光
//	完成日期：
////////////////////////////////////////////////////////////////////////		
void CLogCache::Put(char *pMsg,char *pcLogFileName,long lLogPosition,long nErrorLevel)
{
	char* chrsTmp1;
//	char chrsTmp2[1024];
	//  change by jaylv 2002.02.25	start
	//  将文件名与行号写于正文之后，方便阅读
	chrsTmp1 = new char[strlen(pMsg) + 300];
	if ( chrsTmp1 == NULL)
	{
		return;
	}
	sprintf(chrsTmp1, " %c %s %c File: %s %c line: %ld %c ",m_nSeparator,
		pMsg, m_nSeparator, pcLogFileName, m_nSeparator,lLogPosition,m_nSeparator);
	//  change by jaylv 2002.02.25	end
	Put(chrsTmp1, nErrorLevel);
	delete chrsTmp1;
	chrsTmp1 = NULL;
}

////////////////////////////////////////////////////////////////////////
//	函数说明：	void Flush();
//	函数功能：	将缓冲区内容写入文件中（之前先判断是否有足够硬盘空间）
//	变量说明：	无
//	返回值：	无
//	编写人：	刘骏&张渊&陈学光
//	完成日期：
////////////////////////////////////////////////////////////////////////		
void CLogCache::Flush()
{
	char sLogFileName[LOGCACHE_MAX_PATH] = {0};
	long tNow = time(NULL);
	FILE* pFile = NULL;
	bool bFileExist = false;
	short fResult;
#ifdef WIN32
	ULARGE_INTEGER  i64FreeBytesToCaller;
	ULARGE_INTEGER  i64TotalBytes;
	ULARGE_INTEGER  i64FreeBytes;
	FARPROC pGetDiskFreeSpaceEx;
#endif

	char  lpDirectoryName[3];


	// 缓冲型日志并且日志非空
	if ((m_bCacheMode != true) || (m_nTail == 0)||(m_bInit == false))
		return;

	GenerateFileName(sLogFileName,tNow);

#ifdef WIN32
	//	Add By Chen XueGuang 2002.01.15
	if	(m_nMaxFreeDisk > 0)
	{
		StrNCpy(lpDirectoryName,sLogFileName,3);
		pGetDiskFreeSpaceEx = GetProcAddress( GetModuleHandle("kernel32.dll"),
							 "GetDiskFreeSpaceExA");
		if (pGetDiskFreeSpaceEx)
		{
			fResult = GetDiskFreeSpaceEx(lpDirectoryName, &i64FreeBytesToCaller, &i64TotalBytes, &i64FreeBytes);
			//  change by jaylv 2002.02.25		
			//	应为用户配置的最小写入空间，如用户配置为0，则不限制
			//	此判断在未使用CACHE时是否无效或采用每写入一条记录判断一次？
			ULONGLONG free = m_nMaxFreeDisk;
			if (i64FreeBytes.QuadPart < free)
			{
				return;
			}
		}
	}
	// end of the modification of Chen Xueguang
#endif

	pFile = fopen(sLogFileName,"r");
	if(pFile == NULL)
		bFileExist = false;
	else
	{
		bFileExist = true;
		fclose(pFile);
	}

	pFile = fopen(sLogFileName,"a");
	if(pFile == NULL)
		return;

	if(bFileExist == false)
		WriteFileHead(pFile);
	
	// 资源互斥
	m_hCriticalSection.Lock();

	fwrite(m_pBuffer, sizeof(char), m_nTail, pFile);
	fclose(pFile);

	m_nTail = 0;

	// 离开临界区
	m_hCriticalSection.Unlock();
}

////////////////////////////////////////////////////////////////////////
//	函数说明：	void FlushList();
//	函数功能：	在使用缓冲节点时将缓冲区内容写入文件中
//				（不判断是否有足够硬盘空间）
//	变量说明：	无
//	返回值：	无
//	编写人：	刘骏&张渊
//	完成日期：
////////////////////////////////////////////////////////////////////////		
void CLogCache::FlushList()
{
	char sLogFileName[LOGCACHE_MAX_PATH] = {0};
	long tNow = time(NULL);
	FILE* pFile = NULL;
	bool bFileExist = false;
	CACHE_NODE *pCacheNode = NULL;

	// 缓冲型日志并且日志非空
	if ((m_bCacheMode != true) || (m_nTail == 0) || (m_bInit == false))
		return;

	GenerateFileName(sLogFileName,tNow);

	pFile = fopen(sLogFileName,"r");
	if(pFile == NULL)
		bFileExist = false;
	else
	{
		bFileExist = true;
		fclose(pFile);
	}

	pFile = fopen(sLogFileName,"a");
	if(pFile == NULL)
		return;

	if(bFileExist == false)
		WriteFileHead(pFile);
	
	// 资源互斥
	m_hCriticalSection.Lock();

	pCacheNode = GetCacheBlock();
	while(pCacheNode)
	{
		fwrite(pCacheNode->pBuffer,sizeof(char),pCacheNode->nTail,pFile);
		delete pCacheNode;
		pCacheNode = GetCacheBlock();
	}

	fclose(pFile);

	// 离开临界区
	m_hCriticalSection.Unlock();
}

////////////////////////////////////////////////////////////////////////
//	函数说明：	void FlushAll()
//	函数功能：	写入全部缓冲信息
//	变量说明：	无
//	返回值：	无
//	编写人：	刘骏&张渊
//	完成日期：
////////////////////////////////////////////////////////////////////////		
void CLogCache::FlushAll()
{
	FlushTemp();
	FlushList();
	Flush();
	ClearTemp();
}

////////////////////////////////////////////////////////////////////////
//	函数说明：	void Destroy();
//	函数功能：	实际释放内存函数
//	变量说明：	无
//	返回值：	无
//	编写人：	刘骏&张渊
//	完成日期：
////////////////////////////////////////////////////////////////////////		
void CLogCache::Destroy()
{
	int i;
	//  change by jaylv 2002.02.25  start
	//  在连续释放两次内存时防止错误
	if ( m_bInit == false )
		return;
	//  change by jaylv 2002.02.25  end

	FlushAll();
	m_bInit = false;
	// 释放缓冲区块
	m_hCriticalSection.Lock();
	if(m_pBuffer)
	{
		delete []m_pBuffer;
		m_pBuffer = NULL;
	}

	if(m_pTempBuffer)
	{
		for(i = 0; i< 2* m_nRepeatNum; i++)
		{
			delete []m_pTempBuffer[i];
			m_pTempBuffer[i] = NULL;
		}
		delete []m_pTempBuffer;
		m_pTempBuffer = NULL;
	}


	m_hCriticalSection.Unlock();

	// 释放缓冲区块列表
	m_hCriticalSectionCacheList.Lock();
	while (m_pCacheListHead)
	{
		m_pCacheListTail = m_pCacheListHead->pNext;
		delete m_pCacheListHead;
		m_pCacheListHead = m_pCacheListTail;
	}
	m_hCriticalSectionCacheList.Unlock();

	m_BufMutex.Destroy();

	//delete temporary space
	SAFEDELETE(m_sMsgTemp);
}

////////////////////////////////////////////////////////////////////////
//	函数说明：	void PutCacheBlock(CACHE_NODE *pCacheNode)
//	函数功能：	追加新的内存节点，此节点为列表头
//	变量说明：	*pCacheNode   新的内存节点
//	返回值：	无
//	编写人：	刘骏&张渊
//	完成日期：
////////////////////////////////////////////////////////////////////////		
void CLogCache::PutCacheBlock(CACHE_NODE *pCacheNode)
{
	// 进入临界区
	m_hCriticalSectionCacheList.Lock();

	// 把新节点加到列表头上
	if (m_pCacheListHead)
	{
		pCacheNode->pNext = m_pCacheListHead;
		m_pCacheListHead->pPrev = pCacheNode;
		m_pCacheListHead = pCacheNode;
	}
	else
	{
		m_pCacheListHead = m_pCacheListTail = pCacheNode;
	}

	// 离开临界区
	m_hCriticalSectionCacheList.Unlock();
}

////////////////////////////////////////////////////////////////////////
//	函数说明：	CACHE_NODE* GetCacheBlock()
//	函数功能：	取出当前节点指针
//	变量说明：	
//	返回值：	返回取出的内存块节点指针
//	编写人：	刘骏&张渊
//	完成日期：
////////////////////////////////////////////////////////////////////////		
CACHE_NODE* CLogCache::GetCacheBlock()
{
	CACHE_NODE *pCacheNode;
	
	// 进入临界区
	m_hCriticalSectionCacheList.Lock();

	// 取列表尾节点
	pCacheNode = m_pCacheListTail;

	// 尾节点前移
	if (m_pCacheListTail)
		m_pCacheListTail = m_pCacheListTail->pPrev;

	// 列表是否为空
	if (!m_pCacheListTail)
		m_pCacheListHead = NULL;

	// 离开临界前
	m_hCriticalSectionCacheList.Unlock();

	return pCacheNode;
}

bool CLogCache::Init(int iBufferSize, bool bCacheMode, bool bDailyMode, char* sLogFileName, long lLevel)
{
	return !InitLog(sLogFileName, NULL, NULL, iBufferSize, bCacheMode, bDailyMode ? modeDay : modeOneFile, ' ', lLevel, 1);
}

////////////////////////////////////////////////////////////////////////
//	函数说明：	long InitLog(...)
//	函数功能：	打开普通文本方式存储的Log文件
//	变量说明：	sLogFileName	文件名(不允许为空)
//				sSysName		文件头标识系统名 为空写入[Unknown System]
//				sVersion        文件头标识版本名 为空写入[Unknown Version]
//				nFieldNum		字段数，必须>=1
//				sFieldName		对应字段的字段名,名字间由nSeparator分隔
//								如与字段数不能匹配，则返回错误
//				nBufferSize     缓存区大小  必须>0
//				bCacheMode		是否缓存写入
//				nSaveMode		按何方式显示生成新文件
//				nSeparator		分隔符
//				nErrorLevel		写入日志等级
//				nRepeatNum		写入日志的重复记录数
//				nMaxCacheNum	最大日志节点数
//	返回值：	0						  成功
//				errInvalideParameter      参数错误
//				errAllocateMemory         内存分配错误
//	编写人：	刘骏&张渊
//	完成日期：
////////////////////////////////////////////////////////////////////////		
long CLogCache::InitLog(char* sLogFileName,
						char* sSysName,
						char* sVersion,
						int nBufferSize,
						bool bCacheMode,
						SAVEMODE nSaveMode,
						char nSeparator,
						int nErrorLevel,
						int nRepeatNum,
						int nMaxCacheNum)
{
	long tNow;
	int i;
	int j;
	FILE *pFile = NULL;
	char sFileName[LOGCACHE_MAX_PATH] = {0};

	if (sLogFileName == NULL)
		return errInvalideParameter;

	//  change by xguangchen 2002.02.25
	//  增加对bCacheMode = true限制
	if ((*sLogFileName == '\0') ||
		( ( bCacheMode == true ) && (nBufferSize <= 0) ) )
		return errInvalideParameter;
	 if ( ( bCacheMode == true ) && (nRepeatNum <= 0) )
		return errInvalideParameter;

	strcpy(m_sFileName,sLogFileName);

	if (!sSysName)
		strcpy(m_sSysName,"[Unknown System]");
	else if(sSysName[0])
		strcpy(m_sSysName,sSysName);
	else
		strcpy(m_sSysName,"[Unknown System]");

	if (!sVersion)
		strcpy(m_sVersion,"[Unknown Version]");
	else if(sVersion[0])
		strcpy(m_sVersion,sVersion);
	else
		strcpy(m_sVersion,"[Unknown Version]");

	m_nBufferSize = nBufferSize;
	m_bCacheMode = bCacheMode;
	m_nSeparator = nSeparator;
	m_nBaseErrorLevel = nErrorLevel;
	m_nMaxCacheNum = nMaxCacheNum;

	switch(nSaveMode)
	{
	case modeOneFile:
	case modeDay:
	case modeMonth:
	case modeHour:
	case modeWeek:
		m_nSaveMode = nSaveMode;
		break;
	default:
		return errInvalideParameter;
		break;
	}
	m_nRepeatNum = nRepeatNum;

	//  change by xguangchen 2002.02.25
	if ( bCacheMode == true )
	{
		m_pBuffer = new char[m_nBufferSize + 2];
		if(m_pBuffer == NULL)
		{
			return errAllocateMemory;
		}
	
		m_pTempBuffer = new char* [ 2 * m_nRepeatNum + 2];
		if(m_pTempBuffer == NULL)
		{
			if ( m_pBuffer )
			{
				delete m_pBuffer;
				m_pBuffer = NULL;
			}
			return errAllocateMemory;
		}
		for(i = 0; i< 2* m_nRepeatNum; i++)
		{
			m_pTempBuffer[i] = NULL;
		}
		for(i = 0; i< 2* m_nRepeatNum; i++)
		{
			m_pTempBuffer[i] = new char [m_nBufferSize + 2];
			memset(m_pTempBuffer[i],0,sizeof(char) * m_nBufferSize);
			if(m_pTempBuffer[i] == NULL)
			{
				for(j = 0; j < i ; j++)
				{
					if ( m_pTempBuffer[j] )
					{	
						delete m_pTempBuffer[j];
						m_pTempBuffer[j] = NULL;
					}
				}
				return errAllocateMemory;
			}
		}


	}
	m_nAttrib = attLog;
	m_bInit = true;

	tNow = time(NULL);
	GenerateFileName(sFileName,tNow);
	pFile = fopen(sFileName,"r");
	if(pFile != NULL)
	{
		fclose(pFile);

		pFile = fopen(sFileName,"a");
		if(pFile != NULL)
		{
			WriteFileHead(pFile);

			fclose(pFile);
		}
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////
//	函数说明：	long InitDBLog(...)
//	函数功能：	打开按数据库结构存储的Log文件
//	变量说明：	sLogFileName	文件名(不允许为空)
//				sSysName		文件头标识系统名 为空写入[Unknown System]
//				sVersion        文件头标识版本名 为空写入[Unknown Version]
//				nFieldNum		字段数，必须>=1
//				sFieldName		对应字段的字段名,名字间由nSeparator分隔
//								如与字段数不能匹配，则返回错误
//				nBufferSize     缓存区大小  必须>0
//				bCacheMode		是否缓存写入
//				nSaveMode		按何方式显示生成新文件
//				nSeparator		分隔符
//				nErrorLevel		写入日志等级
//				nMaxCacheNum	最大日志节点数
//	返回值：	0						  成功
//				errInvalideParameter      参数错误
//				errAllocateMemory         内存分配错误
//	编写人：	刘骏&张渊
//	完成日期：
////////////////////////////////////////////////////////////////////////		
long CLogCache::InitDBLog(char *sLogFileName,
						  char *sSysName,
						  char *sVersion,
						  int nFieldNum,
						  char *sFieldName,
						  int nBufferSize,
						  bool bCacheMode,
						  SAVEMODE nSaveMode,
						  char nSeparator,
						  long nErrorLevel,
						  int nMaxCacheNum)
{
	long tNow;
	FILE *pFile = NULL;
	char sFileName[LOGCACHE_MAX_PATH] = {0};

	//  change by xguangchen 2002.02.25
	//  增加对bCacheMode = true限制
	if ((*sLogFileName == '\0') ||
		( ( bCacheMode == true ) && (nBufferSize <= 0) ) )
		return errInvalideParameter;
//	 if ( ( bCacheMode == true ) && (nRepeatNum <= 0) ) )
//		return errInvalideParameter;

	strcpy(m_sFileName,sLogFileName);
	
	if (!sSysName)
		strcpy(m_sSysName,"[Unknown System]");
	else if(sSysName[0])
		strcpy(m_sSysName,sSysName);
	else
		strcpy(m_sSysName,"[Unknown System]");

	if (!sVersion)
		strcpy(m_sVersion,"[Unknown Version]");
	else if(sVersion[0])
		strcpy(m_sVersion,sVersion);
	else
		strcpy(m_sVersion,"[Unknown Version]");

	if(nFieldNum < 1)
		return errInvalideParameter;

	m_nBufferSize = nBufferSize;
	m_bCacheMode = bCacheMode;
	m_nSeparator = nSeparator;
	m_nBaseErrorLevel = nErrorLevel;
	m_nMaxCacheNum = nMaxCacheNum;

	if(sFieldName)
	{
		if(nFieldNum == FindSeparatorNumber(sFieldName) + 1)
		{
			m_nFieldNum = nFieldNum;
			strcpy(m_sFieldName,sFieldName);
		}
		else
			return errInvalideParameter;
	}
	else
		return errInvalideParameter;

	
	switch(nSaveMode)
	{
	case modeOneFile:
	case modeDay:
	case modeMonth:
	case modeHour:
	case modeWeek:
		m_nSaveMode = nSaveMode;
		break;
	default:
		return errInvalideParameter;
		break;
	}
	//  change by jaylv 2002.02.25
	//  对bCacheMode=false 应不初始化此处内存
	if ( bCacheMode == true )
	{			
		m_pBuffer = new char[m_nBufferSize + 1];
	//	m_pTempBuffer = new char[m_nBufferSize];
		if(m_pBuffer == NULL)
		{
			return errAllocateMemory;
		}

		if(m_pTempBuffer == NULL)
		{
			return errAllocateMemory;
		}
	}
	
	m_nAttrib = attDBLog;
	m_bInit = true;

	tNow = time(NULL);
	GenerateFileName(sFileName,tNow);

	pFile = fopen(sFileName,"r");
	if(pFile != NULL)
	{
		fclose(pFile);

		pFile = fopen(sFileName,"a");
		if(pFile != NULL)
		{
			WriteFileHead(pFile);

			fclose(pFile);
		}
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////
//	函数说明：	void GenerateFileName(char* sLogFileName,long tTime);
//	函数功能：	生成新文件名
//	变量说明：	tTime			写入的日期及时间       
//	返回值：	sLogFileName	返回的生成结果文件名
//	编写人：	刘骏&张渊
//	完成日期：
////////////////////////////////////////////////////////////////////////		
void CLogCache::GenerateFileName(char *sLogFileName, long tTime)
{
	struct tm	*pTime = localtime(&tTime);
	long nWeek;
	long nRemain;
	struct tm tBaseDay;

	if(sLogFileName == NULL)
		return;

	switch(m_nSaveMode)
	{
	case modeDay:
		sprintf(sLogFileName,m_sFileName,pTime->tm_year + 1900,pTime->tm_mon + 1,pTime->tm_mday);
		break;
	case modeMonth:
		sprintf(sLogFileName,m_sFileName,pTime->tm_year + 1900,pTime->tm_mon + 1);
		break;
	case modeHour:
		sprintf(sLogFileName,m_sFileName,pTime->tm_year + 1900,pTime->tm_mon + 1,pTime->tm_mday,pTime->tm_hour);
		break;
	case modeWeek:
		nWeek = (pTime->tm_yday + 1) / 7 + 1;
		nRemain = (pTime->tm_yday + 1) % 7;
		
		memset(&tBaseDay,0,sizeof(struct tm));
		tBaseDay.tm_year = pTime->tm_year;
		tBaseDay.tm_mon = 0;
		tBaseDay.tm_mday = 1;
		mktime(&tBaseDay);

		if(nRemain + tBaseDay.tm_wday > 7)
			nWeek ++;

		sprintf(sLogFileName,m_sFileName,pTime->tm_year + 1900,nWeek);
		break;
	case modeOneFile:
		strcpy(sLogFileName,m_sFileName);
		break;
	default:
		break;
	}

//	delete pTime;
}

////////////////////////////////////////////////////////////////////////
//	函数说明：	void WriteFileHead(FILE* pFile);
//	函数功能：	写入日志文件头
//	变量说明：	pFile  文件指针
//	返回值：	无
//	编写人：	刘骏&张渊
//	完成日期：
////////////////////////////////////////////////////////////////////////		
void CLogCache::WriteFileHead(FILE* pFile)
{
	char sFileHead[1024] = {0};
	if(pFile == NULL)
		return;

	sprintf(sFileHead,"# SystemName: %s  Version: %s%s# Date %c Time %c ",
					m_sSysName,m_sVersion,CRLF,m_nSeparator,m_nSeparator);
	
	switch(m_nAttrib)
	{
	case attLog:
		strcat(sFileHead,"LogInfomation");
		break;
	case attDBLog:
		strcat(sFileHead,m_sFieldName);
		break;
	default:
		return;
		break;
	}

	strcat(sFileHead,CRLF);
	fwrite(sFileHead,sizeof(char),strlen(sFileHead),pFile);

}

////////////////////////////////////////////////////////////////////////
//	函数说明：	long FindSeparatorNumber(char *sBuffer);
//	函数功能：	找到指定字符串中分隔符的个数
//	变量说明：	char *sBuffer 欲查找的内容
//	返回值：	无
//	编写人：	刘骏&张渊
//	完成日期：
////////////////////////////////////////////////////////////////////////		
long CLogCache::FindSeparatorNumber(char *sBuffer)
{
	long nNumber = 0;
	long nBufferSize;

	if(sBuffer == NULL)
		return 0;

	nBufferSize = strlen(sBuffer);
	for(long nTemp = 0;nTemp < nBufferSize;nTemp++)
	{
		if(sBuffer[nTemp] == m_nSeparator)
			nNumber ++;
	}

	return nNumber;
}

////////////////////////////////////////////////////////////////////////
//	函数说明：	void SetBaseErrorLevel(long nNewErrorLevel);
//	函数功能：	设置日志记录错误缓冲级别
//	变量说明：	nNewErrorLevel  新的日志错误缓冲级别
//				大于此级别的日志信息将不再记录(一般>=0)			
//	返回值：	无
//	编写人：	陈学光
//	完成日期：
////////////////////////////////////////////////////////////////////////		
void CLogCache::SetBaseErrorLevel(long nNewErrorLevel)
{
	if (nNewErrorLevel < 0)
		m_nBaseErrorLevel = 0;
	else
		m_nBaseErrorLevel = nNewErrorLevel;
}

////////////////////////////////////////////////////////////////////////
//	函数说明：	void SetMaxFreeDisk(int nMaxFreeDisk);
//	函数功能：	设置最大剩余空间(以M为单位)，如硬盘剩余空间小于此值时
//				不再写入日志，仅于m_bCacheMode 为True时有效
//	变量说明：	m_nMaxFreeDisk 新的最大剩余空间
//	返回值：	无
//	编写人：	刘骏&张渊
//	完成日期：
////////////////////////////////////////////////////////////////////////		
void CLogCache::SetMaxFreeDisk(int nMaxFreeDisk)
{
	if (nMaxFreeDisk < 0)
		m_nMaxFreeDisk = 0;
	else
		m_nMaxFreeDisk = nMaxFreeDisk * 1024 * 1024;
}

////////////////////////////////////////////////////////////////////////
//	函数说明：	void SetMaxCacheNum(int nMaxCacheNum);
//	函数功能：	设置最大日志节点
//	变量说明：	nMaxCacheNum 新的最大日志节点
//				此值是否应限制大于0? jaylv 2002.02.22 
//	返回值：	无
//	编写人：	刘骏&张渊
//	完成日期：
////////////////////////////////////////////////////////////////////////		
void CLogCache::SetMaxCacheNum(int nMaxCacheNum)
{
	if ( nMaxCacheNum <= 0 )
	{
		m_nMaxCacheNum = 5;
	}
	else
	{
		m_nMaxCacheNum = nMaxCacheNum;
	}
}

////////////////////////////////////////////////////////////////////////
//	函数说明：	void FlushTemp()
//	函数功能：	将临时内存块写入日志文件
//	变量说明：	无
//	返回值：	无
//	编写人：	陈学光
//	完成日期：
////////////////////////////////////////////////////////////////////////		
void CLogCache::FlushTemp()
{
	char sLogFileName[LOGCACHE_MAX_PATH] = {0};
	long tNow;
	FILE* pFile = NULL;
	bool bFileExist = false;
	char strBuffer[256];
	struct tm *pTime;
	int i;
	bool bRepeat = false;

	// generate timestamp
	tNow = time(NULL);
	pTime = localtime(&tNow);

	sprintf(strBuffer,
			"%04d/%02d/%02d%c%02d:%02d:%02d%c",
			pTime->tm_year + 1900,
			pTime->tm_mon + 1,
			pTime->tm_mday,
			m_nSeparator,
			pTime->tm_hour,
			pTime->tm_min,
			pTime->tm_sec,
			m_nSeparator);

	// generate timestamp string
	GenerateFileName(sLogFileName,tNow);

	// 资源互斥
	m_hCriticalSection.Lock();

	//if the message is too long, return directly
	if(strlen(strBuffer) + strlen(CRLF) >= (unsigned long)m_nBufferSize)
	{
		m_hCriticalSection.Unlock();
		return;
	}

	if ((m_nTail + strlen(strBuffer) + strlen(CRLF)) > (unsigned long)m_nBufferSize)
	{
		//the buffer space remained is too small to contain this message,need to generate a new buffer

		CACHE_NODE *pCacheNode = new CACHE_NODE;
		if(pCacheNode == NULL)
		{
			m_hCriticalSection.Unlock();
			return;
		}

		pCacheNode->nTail = m_nTail;
		pCacheNode->pBuffer = m_pBuffer;

		// 生成新缓冲区块
		m_pBuffer = new char[m_nBufferSize + 1];
		if (!m_pBuffer)
		{
			m_pBuffer = pCacheNode->pBuffer;
			delete pCacheNode;
			m_hCriticalSection.Unlock();
			return;
		}
		m_nTail = 0;

		// 加入缓冲区块列表
		PutCacheBlock(pCacheNode);
	}
	if(m_bCacheMode == true)
	{
		for (i = 0;i < 2*m_nRepeatNum; i++)
		{
			if (m_pTempBuffer[i][0] != 0)
			{
				strcpy(m_pBuffer + m_nTail,strBuffer);
				m_nTail += strlen(strBuffer);
				strcpy(m_pBuffer + m_nTail,m_pTempBuffer[i]);
				m_nTail += strlen(m_pTempBuffer[i]);
				strcpy(m_pBuffer + m_nTail,CRLF);
				m_nTail += strlen(CRLF);
			}
		}
	}
		
	// 离开临界区
	m_hCriticalSection.Unlock();

}

////////////////////////////////////////////////////////////////////////
//	函数说明：	void ClearTemp()
//	函数功能：	清除临时内存内容
//	变量说明：	无
//	返回值：	无
//	编写人：	陈学光
//	完成日期：
////////////////////////////////////////////////////////////////////////		
void CLogCache::ClearTemp()
{
	int i;
	if (m_bCacheMode == true)
	{
		for (i = 0;i < 2*m_nRepeatNum; i++)
		{
			memset(m_pTempBuffer[i],0,sizeof(char)*m_nBufferSize);
		}
	}
}

