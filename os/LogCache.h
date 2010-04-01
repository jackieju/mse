/**
 * LogCache.h - head file for log output module
 * @author: Liu Jun
 * @author: Chen Xueguang
 * @package SSLib
 * @version 1.0.0
 * @Date  : 2001-5-28
 */

#ifndef __LOGCACHE_HEADER__
#define __LOGCACHE_HEADER__

//
//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//	使用实例
//	long iBack, i;
//
//	iBack = cDebug.InitLog("D:\\Log\\Debug%d%d%d.txt", "Test Mem Table", "1.0.0-0001-B", 2048, true, CLogCache::modeDay , ' ', 0, 1, 5);	
//	if ( iBack != 0 )
//	{
//		printf(" Init Log Name: <D:\\Log\\Debug> Error, ErrCode<%d> ", iBack);
//		return;
//	}
//
//	cDebug.SetBaseErrorLevel(2);
//	cDebug.SetMaxFreeDisk(0);	
//	cDebug.Put("Test Size aaaa", __FILE__, __LINE__, 2);
//	cDebug.Put("Test Size bbb", 0);
//
//	cDebug.FlushAll();
//	cDebug.Destroy();
////////////////////////////////////////////////////////////////////////		

#ifdef WIN32
 #include <windows.h>
 #include <Winbase.h>      //  因需查询磁盘空间使用
 #include <Shlwapi.h>
#endif

#include "LogBase.h"
#include "CSS_LOCKEX.h"

#define TIMESTAMP_LENGTH			22			// 时间标记长度 1999/06/23 15:00:00 - 
#define CRLF						"\r\n"					// 回车换行
#define TEMPMSG_LEN					65536
#define MAX_FORMAT_LEN				200

#define LOG_PERMANENT		0
#define LOG_FATAL_ERROR		10	
#define	LOG_ESSENTIAL		5
#define LOG_GENERAL_ERROR	50
#define LOG_WARING			100		
#define LOG_NORMAL_INFO		200

#ifdef WIN32
 #define	STATIC_LOG_FILE				"C:/SYSLOG.TXT"
#else
 #define	STATIC_LOG_FILE				"~/SYSLOG.TXT"
#endif

// for log information output
#define LOGPUT(x, y, z) \
	do { \
		CLogCache *pL = &(x); \
		char *p = strrchr(__FILE__, '\\'); \
		if (p) p ++; \
		pL->Put(y,  p == NULL ? "unkown file" : p, __LINE__, z); \
		} while(0)


typedef struct CACHE_NODE
{
	int nTail;
	char *pBuffer;
	struct CACHE_NODE *pNext;
	struct CACHE_NODE *pPrev;

	CACHE_NODE()
	{
		nTail = 0;
		pNext = pPrev = NULL;
		pBuffer = NULL;
	}

	~CACHE_NODE()
	{
		if(pBuffer)
			delete []pBuffer;
	}

} CACHE_NODE;

////////////////////////////////////////////////////////////////////////
//	对象名称：	 日志记录类
//	对象描述：   将字符串写入日志文件
//	重要函数：   
// 	
//	编写人：     刘骏&张渊&陈学光
//	完成日期：   
////////////////////////////////////////////////////////////////////////
/**
 * 日志缓冲类
 */

class OSLIB_DLL_FUNC_HEAD CLogCache : public CLogBase
{
//		日志缓冲区大小	
#define LOGCACHE_BUFFERSIZE		2048
#define LOGCACHE_LOGFILENAME	"Log.Log"
#define LOGCACHE_EXTLOGFILENAME	"Ext.Log"

#define LOGCACHE_FIELD_DATE		"Date"
#define LOGCACHE_FIELD_TIME		"Time"
#define LOGCACHE_FIELD_LOG		"Log"

//		日志路径最大长度	
#define LOGCACHE_MAX_PATH	260

public:

	enum{
		errInvalideParameter = 1024,
		errAllocateMemory
	};

	enum{
		attLog = 2048,
		attDBLog
	};	
private:
	long m_nBaseErrorLevel;							// ErrorLevel,写入错误级别,如写入错误信息
													// 级别高于此设定值，则不写入 	
													// 对 m_bCacheMode =false时无效
	char m_sFieldName[1024];						// DBLog的日志字段名称
	long m_nFieldNum;								// DBLog的日志字段数量
	long m_nSaveMode;								// 保存模式（每天使用新文件、每小时...）
													// 由enum SAVEMODE定义		
	char m_sSysName[1024];							// 系统名称
	char m_sVersion[1024];							// 系统版本信息
	bool m_bInit;									// 是否成功初始化
	bool m_bCacheMode;								// 是否缓冲
	int m_nAttrib;									// 日志是维护日志还是访问日志
	char m_nSeparator;								// 分隔符
	int m_nTail;									// 日志信息尾
	int m_nBufferSize;								// 日志缓冲区大小
	char *m_pBuffer;								// 日志缓冲区指针

	char **m_pTempBuffer;							// 存放临时日志文件的地方
	int m_nRepeatNum;								// 重复的个数
	
	char m_sFileName[LOGCACHE_MAX_PATH];			// 日志文件名
	int m_nMaxCacheNum;								// 最大日志节点数

	long m_nMaxFreeDisk;							//	允许的最大硬盘空间
	long m_nCount;									//	计数器

	char	*m_sMsgTemp;
	CSS_MUTEX m_BufMutex;	// yangyan add 2002-08-15, protect m_sMsgTemp buffer

	CSS_MUTEX m_hCriticalSection;						// 并发访问锁
	CSS_MUTEX m_hCriticalSectionCacheList;				// 列表互斥


	CACHE_NODE *m_pCacheListHead;					// 日志信息块列表
	CACHE_NODE *m_pCacheListTail;					// 日志信息块列表

public:
	////////////////////////////////////////////////////////////////////////
	//	函数说明：	CLogCache();
	//	函数功能：	构造函数
	//	变量说明：	无
	//	返回值：	无
	//	编写人：	刘骏&张渊&陈学光
	//	完成日期：
	////////////////////////////////////////////////////////////////////////		
	CLogCache();

	////////////////////////////////////////////////////////////////////////
	//	函数说明：	~CLogCache();
	//	函数功能：	析构函数
	//	变量说明：	无
	//	返回值：	无
	//	编写人：	刘骏&张渊&陈学光
	//	完成日期：
	////////////////////////////////////////////////////////////////////////		
	~CLogCache();

	////////////////////////////////////////////////////////////////////////
	//	函数说明：	void SetBaseErrorLevel(long nNewErrorLevel);
	//	函数功能：	设置日志记录错误缓冲级别
	//	变量说明：	nNewErrorLevel  新的日志错误缓冲级别
	//				大于此级别的日志信息将不再记录(一般>=0)			
	//	返回值：	无
	//	编写人：	陈学光
	//	完成日期：
	////////////////////////////////////////////////////////////////////////		
	void SetBaseErrorLevel(long nNewErrorLevel);

	////////////////////////////////////////////////////////////////////////
	//	函数说明：	void SetMaxCacheNum(int nMaxCacheNum);
	//	函数功能：	设置最大日志节点
	//	变量说明：	nMaxCacheNum 新的最大日志节点
	//				此值必须>0 如<= 0，写入5
	//	返回值：	无
	//	编写人：	刘骏&张渊
	//	完成日期：
	////////////////////////////////////////////////////////////////////////		
	void SetMaxCacheNum(int nMaxCacheNum = 5);

	////////////////////////////////////////////////////////////////////////
	//	函数说明：	void SetMaxFreeDisk(int nMaxFreeDisk);
	//	函数功能：	设置最大剩余空间(以M为单位)，如硬盘剩余空间小于此值时
	//				不再写入日志，仅于m_bCacheMode 为True时有效
	//	变量说明：	m_nMaxFreeDisk 新的最大剩余空间
	//	返回值：	无
	//	编写人：	刘骏&张渊
	//	完成日期：
	////////////////////////////////////////////////////////////////////////		
	void SetMaxFreeDisk(int nMaxFreeDisk);

	////////////////////////////////////////////////////////////////////////
	//	函数说明：	long FindSeparatorNumber(char *sBuffer);
	//	函数功能：	找到指定字符串中分隔符的个数
	//	变量说明：	sBuffer 欲查找的内容
	//	返回值：	无
	//	编写人：	刘骏&张渊
	//	完成日期：
	////////////////////////////////////////////////////////////////////////		
	long FindSeparatorNumber(char *sBuffer);

	////////////////////////////////////////////////////////////////////////
	//	函数说明：	void WriteFileHead(FILE* pFile);
	//	函数功能：	写入日志文件头
	//	变量说明：	pFile  文件指针
	//	返回值：	无
	//	编写人：	刘骏&张渊
	//	完成日期：
	////////////////////////////////////////////////////////////////////////		
	void WriteFileHead(FILE* pFile);

	////////////////////////////////////////////////////////////////////////
	//	函数说明：	void GenerateFileName(char* sLogFileName,long tTime);
	//	函数功能：	生成新文件名
	//	变量说明：	tTime			写入的日期及时间       
	//	返回值：	sLogFileName	返回的生成结果文件名
	//	编写人：	刘骏&张渊
	//	完成日期：
	////////////////////////////////////////////////////////////////////////		
	void GenerateFileName(char* sLogFileName,long tTime);

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
	long InitDBLog(char* sLogFileName,
				   char* sSysName = NULL,
				   char* sVersion = NULL,
				   int nFieldNum = 1,
				   char* sFieldName = "LOGITEM", 
				   int nBufferSize = LOGCACHE_BUFFERSIZE,
				   bool bCacheMode = true,
				   SAVEMODE nSaveMode = modeOneFile, 
				   char nSeparator = ' ',
				   long nErrorLevel = 0,
				   int nMaxCacheNum = 5);

	/**
	 * <pre>
	 * Initilization of Log Cache class
	 * Parameters:
	 *				int, [in], initial buffer size
	 *				bool, [in], cache mode means cache or not
	 *				bool, [in], whether one file per day or not
	 *				char *, [in], logfile name
	 * Return values:
	 *				true - success, false - failure
	 * </pre>
	 */
	bool Init(int iBufferSize, bool bCacheMode, bool bDaily, char* sLogFileName, long lLevel = 10000);

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
	long InitLog(char* sLogFileName = LOGCACHE_LOGFILENAME,
				 char* sSysName = NULL,
				 char* sVersion = NULL,
				 int nBufferSize = LOGCACHE_BUFFERSIZE,
				 bool bCacheMode = true,
				 SAVEMODE nSaveMode = modeDay, 
				 char nSeparator = ' ',
				 int nErrorLevel = 0,
				 int nRepeatNum = 0,
				 int nMaxCacheNum = 5);

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
	void Put(char *pMsg,long nErrorLevel = 0);

	////////////////////////////////////////////////////////////////////////
	//	函数说明：	void Put(char *pMsg,long nErrorLevel = 0);
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
	void Put(char *pMsg,char *pcLogFileName,long lLogPosition,long nErrorLevel = 0);

	/**
	 * <pre>
	 * Put log information into memory or disk.
	 * Parameters:
	 *				char *, [in], log information
	 * Return values:
	 *				None
	 * </pre>
	 */
	void Put(long level, char* file, long line, char* format,...);

	/**
	 * <pre>
	 * Put log information into memory or disk.
	 * Parameters:
	 *				char *, [in], log information
	 * Return values:
	 *				None
	 * </pre>
	 */
	static void StaticPut(char* filename, char* msg, char* sourcefile, long line, char* format = NULL);

	////////////////////////////////////////////////////////////////////////
	//	函数说明：	void Flush();
	//	函数功能：	将缓冲区内容写入文件中（之前先判断是否有足够硬盘空间）
	//	变量说明：	无
	//	返回值：	无
	//	编写人：	刘骏&张渊&陈学光
	//	完成日期：
	////////////////////////////////////////////////////////////////////////		
	void Flush();

	////////////////////////////////////////////////////////////////////////
	//	函数说明：	void FlushList();
	//	函数功能：	在使用缓冲节点时将缓冲区内容写入文件中
	//				（不判断是否有足够硬盘空间）
	//	变量说明：	无
	//	返回值：	无
	//	编写人：	刘骏&张渊
	//	完成日期：
	////////////////////////////////////////////////////////////////////////		
	void FlushList();
	
	////////////////////////////////////////////////////////////////////////
	//	函数说明：	void FlushAll()
	//	函数功能：	写入全部缓冲信息
	//	变量说明：	无
	//	返回值：	无
	//	编写人：	刘骏&张渊
	//	完成日期：
	////////////////////////////////////////////////////////////////////////		
	void FlushAll();

	////////////////////////////////////////////////////////////////////////
	//	函数说明：	void Destroy();
	//	函数功能：	实际释放内存函数
	//	变量说明：	无
	//	返回值：	无
	//	编写人：	刘骏&张渊
	//	完成日期：
	////////////////////////////////////////////////////////////////////////		
	void Destroy();
	
	////////////////////////////////////////////////////////////////////////
	//	函数说明：	void PutCacheBlock(CACHE_NODE *pCacheNode)
	//	函数功能：	追加新的内存节点，此节点为列表头
	//	变量说明：	*pCacheNode   新的内存节点
	//	返回值：	无
	//	编写人：	刘骏&张渊
	//	完成日期：
	////////////////////////////////////////////////////////////////////////		
	void PutCacheBlock(CACHE_NODE *pCacheNode);

	////////////////////////////////////////////////////////////////////////
	//	函数说明：	CACHE_NODE* GetCacheBlock()
	//	函数功能：	取出当前节点指针
	//	变量说明：	
	//	返回值：	返回取出的内存块节点指针
	//	编写人：	刘骏&张渊
	//	完成日期：
	////////////////////////////////////////////////////////////////////////		
	CACHE_NODE *GetCacheBlock();

	////////////////////////////////////////////////////////////////////////
	//	函数说明：	void FlushTemp()
	//	函数功能：	将临时内存块写入日志文件
	//	变量说明：	无
	//	返回值：	无
	//	编写人：	刘骏&张渊
	//	完成日期：
	////////////////////////////////////////////////////////////////////////		
	void FlushTemp();

	////////////////////////////////////////////////////////////////////////
	//	函数说明：	void ClearTemp()
	//	函数功能：	清除临时内存内容
	//	变量说明：	无
	//	返回值：	无
	//	编写人：	陈学光
	//	完成日期：
	////////////////////////////////////////////////////////////////////////		
	void ClearTemp();
};

#endif // end of header file
