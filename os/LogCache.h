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
//	ʹ��ʵ��
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
 #include <Winbase.h>      //  �����ѯ���̿ռ�ʹ��
 #include <Shlwapi.h>
#endif

#include "LogBase.h"
#include "CSS_LOCKEX.h"

#define TIMESTAMP_LENGTH			22			// ʱ���ǳ��� 1999/06/23 15:00:00 - 
#define CRLF						"\r\n"					// �س�����
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
//	�������ƣ�	 ��־��¼��
//	����������   ���ַ���д����־�ļ�
//	��Ҫ������   
// 	
//	��д�ˣ�     ����&��Ԩ&��ѧ��
//	������ڣ�   
////////////////////////////////////////////////////////////////////////
/**
 * ��־������
 */

class OSLIB_DLL_FUNC_HEAD CLogCache : public CLogBase
{
//		��־��������С	
#define LOGCACHE_BUFFERSIZE		2048
#define LOGCACHE_LOGFILENAME	"Log.Log"
#define LOGCACHE_EXTLOGFILENAME	"Ext.Log"

#define LOGCACHE_FIELD_DATE		"Date"
#define LOGCACHE_FIELD_TIME		"Time"
#define LOGCACHE_FIELD_LOG		"Log"

//		��־·����󳤶�	
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
	long m_nBaseErrorLevel;							// ErrorLevel,д����󼶱�,��д�������Ϣ
													// ������ڴ��趨ֵ����д�� 	
													// �� m_bCacheMode =falseʱ��Ч
	char m_sFieldName[1024];						// DBLog����־�ֶ�����
	long m_nFieldNum;								// DBLog����־�ֶ�����
	long m_nSaveMode;								// ����ģʽ��ÿ��ʹ�����ļ���ÿСʱ...��
													// ��enum SAVEMODE����		
	char m_sSysName[1024];							// ϵͳ����
	char m_sVersion[1024];							// ϵͳ�汾��Ϣ
	bool m_bInit;									// �Ƿ�ɹ���ʼ��
	bool m_bCacheMode;								// �Ƿ񻺳�
	int m_nAttrib;									// ��־��ά����־���Ƿ�����־
	char m_nSeparator;								// �ָ���
	int m_nTail;									// ��־��Ϣβ
	int m_nBufferSize;								// ��־��������С
	char *m_pBuffer;								// ��־������ָ��

	char **m_pTempBuffer;							// �����ʱ��־�ļ��ĵط�
	int m_nRepeatNum;								// �ظ��ĸ���
	
	char m_sFileName[LOGCACHE_MAX_PATH];			// ��־�ļ���
	int m_nMaxCacheNum;								// �����־�ڵ���

	long m_nMaxFreeDisk;							//	��������Ӳ�̿ռ�
	long m_nCount;									//	������

	char	*m_sMsgTemp;
	CSS_MUTEX m_BufMutex;	// yangyan add 2002-08-15, protect m_sMsgTemp buffer

	CSS_MUTEX m_hCriticalSection;						// ����������
	CSS_MUTEX m_hCriticalSectionCacheList;				// �б���


	CACHE_NODE *m_pCacheListHead;					// ��־��Ϣ���б�
	CACHE_NODE *m_pCacheListTail;					// ��־��Ϣ���б�

public:
	////////////////////////////////////////////////////////////////////////
	//	����˵����	CLogCache();
	//	�������ܣ�	���캯��
	//	����˵����	��
	//	����ֵ��	��
	//	��д�ˣ�	����&��Ԩ&��ѧ��
	//	������ڣ�
	////////////////////////////////////////////////////////////////////////		
	CLogCache();

	////////////////////////////////////////////////////////////////////////
	//	����˵����	~CLogCache();
	//	�������ܣ�	��������
	//	����˵����	��
	//	����ֵ��	��
	//	��д�ˣ�	����&��Ԩ&��ѧ��
	//	������ڣ�
	////////////////////////////////////////////////////////////////////////		
	~CLogCache();

	////////////////////////////////////////////////////////////////////////
	//	����˵����	void SetBaseErrorLevel(long nNewErrorLevel);
	//	�������ܣ�	������־��¼���󻺳弶��
	//	����˵����	nNewErrorLevel  �µ���־���󻺳弶��
	//				���ڴ˼������־��Ϣ�����ټ�¼(һ��>=0)			
	//	����ֵ��	��
	//	��д�ˣ�	��ѧ��
	//	������ڣ�
	////////////////////////////////////////////////////////////////////////		
	void SetBaseErrorLevel(long nNewErrorLevel);

	////////////////////////////////////////////////////////////////////////
	//	����˵����	void SetMaxCacheNum(int nMaxCacheNum);
	//	�������ܣ�	���������־�ڵ�
	//	����˵����	nMaxCacheNum �µ������־�ڵ�
	//				��ֵ����>0 ��<= 0��д��5
	//	����ֵ��	��
	//	��д�ˣ�	����&��Ԩ
	//	������ڣ�
	////////////////////////////////////////////////////////////////////////		
	void SetMaxCacheNum(int nMaxCacheNum = 5);

	////////////////////////////////////////////////////////////////////////
	//	����˵����	void SetMaxFreeDisk(int nMaxFreeDisk);
	//	�������ܣ�	�������ʣ��ռ�(��MΪ��λ)����Ӳ��ʣ��ռ�С�ڴ�ֵʱ
	//				����д����־������m_bCacheMode ΪTrueʱ��Ч
	//	����˵����	m_nMaxFreeDisk �µ����ʣ��ռ�
	//	����ֵ��	��
	//	��д�ˣ�	����&��Ԩ
	//	������ڣ�
	////////////////////////////////////////////////////////////////////////		
	void SetMaxFreeDisk(int nMaxFreeDisk);

	////////////////////////////////////////////////////////////////////////
	//	����˵����	long FindSeparatorNumber(char *sBuffer);
	//	�������ܣ�	�ҵ�ָ���ַ����зָ����ĸ���
	//	����˵����	sBuffer �����ҵ�����
	//	����ֵ��	��
	//	��д�ˣ�	����&��Ԩ
	//	������ڣ�
	////////////////////////////////////////////////////////////////////////		
	long FindSeparatorNumber(char *sBuffer);

	////////////////////////////////////////////////////////////////////////
	//	����˵����	void WriteFileHead(FILE* pFile);
	//	�������ܣ�	д����־�ļ�ͷ
	//	����˵����	pFile  �ļ�ָ��
	//	����ֵ��	��
	//	��д�ˣ�	����&��Ԩ
	//	������ڣ�
	////////////////////////////////////////////////////////////////////////		
	void WriteFileHead(FILE* pFile);

	////////////////////////////////////////////////////////////////////////
	//	����˵����	void GenerateFileName(char* sLogFileName,long tTime);
	//	�������ܣ�	�������ļ���
	//	����˵����	tTime			д������ڼ�ʱ��       
	//	����ֵ��	sLogFileName	���ص����ɽ���ļ���
	//	��д�ˣ�	����&��Ԩ
	//	������ڣ�
	////////////////////////////////////////////////////////////////////////		
	void GenerateFileName(char* sLogFileName,long tTime);

	////////////////////////////////////////////////////////////////////////
	//	����˵����	long InitDBLog(...)
	//	�������ܣ�	�򿪰����ݿ�ṹ�洢��Log�ļ�
	//	����˵����	sLogFileName	�ļ���(������Ϊ��)
	//				sSysName		�ļ�ͷ��ʶϵͳ�� Ϊ��д��[Unknown System]
	//				sVersion        �ļ�ͷ��ʶ�汾�� Ϊ��д��[Unknown Version]
	//				nFieldNum		�ֶ���������>=1
	//				sFieldName		��Ӧ�ֶε��ֶ���,���ּ���nSeparator�ָ�
	//								�����ֶ�������ƥ�䣬�򷵻ش���
	//				nBufferSize     ��������С  ����>0
	//				bCacheMode		�Ƿ񻺴�д��
	//				nSaveMode		���η�ʽ��ʾ�������ļ�
	//				nSeparator		�ָ���
	//				nErrorLevel		д����־�ȼ�
	//				nMaxCacheNum	�����־�ڵ���
	//	����ֵ��	0						  �ɹ�
	//				errInvalideParameter      ��������
	//				errAllocateMemory         �ڴ�������
	//	��д�ˣ�	����&��Ԩ
	//	������ڣ�
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
	//	����˵����	long InitLog(...)
	//	�������ܣ�	����ͨ�ı���ʽ�洢��Log�ļ�
	//	����˵����	sLogFileName	�ļ���(������Ϊ��)
	//				sSysName		�ļ�ͷ��ʶϵͳ�� Ϊ��д��[Unknown System]
	//				sVersion        �ļ�ͷ��ʶ�汾�� Ϊ��д��[Unknown Version]
	//				nFieldNum		�ֶ���������>=1
	//				sFieldName		��Ӧ�ֶε��ֶ���,���ּ���nSeparator�ָ�
	//								�����ֶ�������ƥ�䣬�򷵻ش���
	//				nBufferSize     ��������С  ����>0
	//				bCacheMode		�Ƿ񻺴�д��
	//				nSaveMode		���η�ʽ��ʾ�������ļ�
	//				nSeparator		�ָ���
	//				nErrorLevel		д����־�ȼ�
	//				nRepeatNum		д����־���ظ���¼��
	//				nMaxCacheNum	�����־�ڵ���
	//	����ֵ��	0						  �ɹ�
	//				errInvalideParameter      ��������
	//				errAllocateMemory         �ڴ�������
	//	��д�ˣ�	����&��Ԩ
	//	������ڣ�
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
	//	����˵����	void Put(char *pMsg,long nErrorLevel = 0);
	//	�������ܣ�	д���ı���־����
	//	����˵����	pMsg            ��־�ַ���
	//				nErrorLevel     ��־�ȼ���¼,�����m_nBaseErrorLevel
	//								��д����־
	//	����ֵ��	��
	//	��д�ˣ�	����&��Ԩ&��ѧ��
	//	������ڣ�
	////////////////////////////////////////////////////////////////////////		
	void Put(char *pMsg,long nErrorLevel = 0);

	////////////////////////////////////////////////////////////////////////
	//	����˵����	void Put(char *pMsg,long nErrorLevel = 0);
	//	�������ܣ�	д���ı���־���ݣ������к�ѡ��
	//	����˵����	pMsg            ��־�ַ���
	//				pcLogFileName   ������־��Դ�ļ���
	//				lLogPosition	������־���к�
	//				nErrorLevel     ��־�ȼ���¼,�����m_nBaseErrorLevel
	//								��д����־
	//	����ֵ��	��
	//	��д�ˣ�	����&��Ԩ&��ѧ��
	//	������ڣ�
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
	//	����˵����	void Flush();
	//	�������ܣ�	������������д���ļ��У�֮ǰ���ж��Ƿ����㹻Ӳ�̿ռ䣩
	//	����˵����	��
	//	����ֵ��	��
	//	��д�ˣ�	����&��Ԩ&��ѧ��
	//	������ڣ�
	////////////////////////////////////////////////////////////////////////		
	void Flush();

	////////////////////////////////////////////////////////////////////////
	//	����˵����	void FlushList();
	//	�������ܣ�	��ʹ�û���ڵ�ʱ������������д���ļ���
	//				�����ж��Ƿ����㹻Ӳ�̿ռ䣩
	//	����˵����	��
	//	����ֵ��	��
	//	��д�ˣ�	����&��Ԩ
	//	������ڣ�
	////////////////////////////////////////////////////////////////////////		
	void FlushList();
	
	////////////////////////////////////////////////////////////////////////
	//	����˵����	void FlushAll()
	//	�������ܣ�	д��ȫ��������Ϣ
	//	����˵����	��
	//	����ֵ��	��
	//	��д�ˣ�	����&��Ԩ
	//	������ڣ�
	////////////////////////////////////////////////////////////////////////		
	void FlushAll();

	////////////////////////////////////////////////////////////////////////
	//	����˵����	void Destroy();
	//	�������ܣ�	ʵ���ͷ��ڴ溯��
	//	����˵����	��
	//	����ֵ��	��
	//	��д�ˣ�	����&��Ԩ
	//	������ڣ�
	////////////////////////////////////////////////////////////////////////		
	void Destroy();
	
	////////////////////////////////////////////////////////////////////////
	//	����˵����	void PutCacheBlock(CACHE_NODE *pCacheNode)
	//	�������ܣ�	׷���µ��ڴ�ڵ㣬�˽ڵ�Ϊ�б�ͷ
	//	����˵����	*pCacheNode   �µ��ڴ�ڵ�
	//	����ֵ��	��
	//	��д�ˣ�	����&��Ԩ
	//	������ڣ�
	////////////////////////////////////////////////////////////////////////		
	void PutCacheBlock(CACHE_NODE *pCacheNode);

	////////////////////////////////////////////////////////////////////////
	//	����˵����	CACHE_NODE* GetCacheBlock()
	//	�������ܣ�	ȡ����ǰ�ڵ�ָ��
	//	����˵����	
	//	����ֵ��	����ȡ�����ڴ��ڵ�ָ��
	//	��д�ˣ�	����&��Ԩ
	//	������ڣ�
	////////////////////////////////////////////////////////////////////////		
	CACHE_NODE *GetCacheBlock();

	////////////////////////////////////////////////////////////////////////
	//	����˵����	void FlushTemp()
	//	�������ܣ�	����ʱ�ڴ��д����־�ļ�
	//	����˵����	��
	//	����ֵ��	��
	//	��д�ˣ�	����&��Ԩ
	//	������ڣ�
	////////////////////////////////////////////////////////////////////////		
	void FlushTemp();

	////////////////////////////////////////////////////////////////////////
	//	����˵����	void ClearTemp()
	//	�������ܣ�	�����ʱ�ڴ�����
	//	����˵����	��
	//	����ֵ��	��
	//	��д�ˣ�	��ѧ��
	//	������ڣ�
	////////////////////////////////////////////////////////////////////////		
	void ClearTemp();
};

#endif // end of header file
