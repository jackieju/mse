#ifndef __COMMONLIB_LOG__
#define __COMMONLIB_LOG__

//		��־����Ƶ�ʶ���	
enum SAVEMODE{
	modeOneFile = 0,
		modeDay,
		modeMonth,
		modeHour,
		modeWeek,
		modeNumber
};


/**
	<PRE>	
	�������ƣ�	 ��־��¼��
	����������   ���ַ���д����־�ļ�
	��Ҫ������   
	��д�ˣ�     ����&��Ԩ&��ѧ��
	������ڣ�   
	</PRE>	
*/
class CLogBase 
{
public:
	/**
		<PRE>	
		����˵����	void SetBaseErrorLevel(long nNewErrorLevel);
		�������ܣ�	������־��¼���󻺳弶��
		����˵����	nNewErrorLevel  �µ���־���󻺳弶��
					���ڴ˼������־��Ϣ�����ټ�¼(һ��>=0)			
		����ֵ��	��
		��д�ˣ�	��ѧ��
		������ڣ�
		</PRE>	
	*/
	virtual void SetBaseErrorLevel(long nNewErrorLevel) = 0;

	/**
		<PRE>	
		����˵����	void SetMaxCacheNum(int nMaxCacheNum);
		�������ܣ�	���������־�ڵ�
		����˵����	nMaxCacheNum �µ������־�ڵ�
					��ֵ����>0 ��<= 0��д��5
		����ֵ��	��
		��д�ˣ�	����&��Ԩ
		������ڣ�
		</PRE>	
	*/
	virtual void SetMaxCacheNum(int nMaxCacheNum = 5) = 0;

	/**
		<PRE>	
		����˵����	void SetMaxFreeDisk(int nMaxFreeDisk);
		�������ܣ�	�������ʣ��ռ�(��MΪ��λ)����Ӳ��ʣ��ռ�С�ڴ�ֵʱ
					����д����־������m_bCacheMode ΪTrueʱ��Ч
		����˵����	m_nMaxFreeDisk �µ����ʣ��ռ�
		����ֵ��	��
		��д�ˣ�	����&��Ԩ
		������ڣ�
		</PRE>	
	*/
	virtual void SetMaxFreeDisk(int nMaxFreeDisk) = 0;


	/**
		<PRE>	
		����˵����	long FindSeparatorNumber(char *sBuffer);
		�������ܣ�	�ҵ�ָ���ַ����зָ����ĸ���
		����˵����	sBuffer �����ҵ�����
		����ֵ��	��
		��д�ˣ�	����&��Ԩ
		������ڣ�
		</PRE>
	*/
	virtual long FindSeparatorNumber(char *sBuffer) = 0;

	/**
		<PRE>	
		����˵����	void GenerateFileName(char* sLogFileName,long tTime);
		�������ܣ�	�������ļ���
		����˵����	tTime			д������ڼ�ʱ��       
		����ֵ��	sLogFileName	���ص����ɽ���ļ���
		��д�ˣ�	����&��Ԩ
		������ڣ�
		</PRE>	
	*/
	virtual void GenerateFileName(char* sLogFileName,long tTime) = 0;

	/**
		<PRE>	
		����˵����	long InitLog(...)
		�������ܣ�	����ͨ�ı���ʽ�洢��Log�ļ�
		����˵����	sLogFileName	�ļ���(������Ϊ��)
					sSysName		�ļ�ͷ��ʶϵͳ�� Ϊ��д��[Unknown System]
					sVersion        �ļ�ͷ��ʶ�汾�� Ϊ��д��[Unknown Version]
					nFieldNum		�ֶ���������>=1
					sFieldName		��Ӧ�ֶε��ֶ���,���ּ���nSeparator�ָ�
									�����ֶ�������ƥ�䣬�򷵻ش���
					nBufferSize     ��������С  ����>0
					bCacheMode		�Ƿ񻺴�д��
					nSaveMode		���η�ʽ��ʾ�������ļ�
					nSeparator		�ָ���
					nErrorLevel		д����־�ȼ�
					nRepeatNum		д����־���ظ���¼��
					nMaxCacheNum	�����־�ڵ���
		����ֵ��	0						  �ɹ�
					errInvalideParameter      ��������
					errAllocateMemory         �ڴ�������
		��д�ˣ�	����&��Ԩ
		������ڣ�
		</PRE>	
	*/
	virtual long InitLog(char* sLogFileName,
							 char* sSysName,
							 char* sVersion,
							int nBufferSize,
							bool bCacheMode,
							SAVEMODE nSaveMode,
							char nSeparator,
							int nErrorLevel,
							 int nRepeatNum,
						   int nMaxCacheNum) = 0;

	/**
		<PRE>	
		����˵����	void Put(char *pMsg,long nErrorLevel = 0);
		�������ܣ�	д���ı���־����
		����˵����	pMsg            ��־�ַ���
					nErrorLevel     ��־�ȼ���¼,�����m_nBaseErrorLevel
									��д����־
		����ֵ��	��
		��д�ˣ�	����&��Ԩ&��ѧ��
		������ڣ�
		</PRE>	
	*/
	virtual void Put(char *pMsg,long nErrorLevel = 0) = 0;

	/**
		<PRE>	
		����˵����	void Put(char *pMsg,long nErrorLevel = 0);
		�������ܣ�	д���ı���־���ݣ������к�ѡ��
		����˵����	pMsg            ��־�ַ���
					pcLogFileName   ������־��Դ�ļ���
					lLogPosition	������־���к�
					nErrorLevel     ��־�ȼ���¼,�����m_nBaseErrorLevel
									��д����־
		����ֵ��	��
		��д�ˣ�	����&��Ԩ&��ѧ��
		������ڣ�
		</PRE>	
	*/
	virtual void Put(char *pMsg,char *pcLogFileName,long lLogPosition,long nErrorLevel = 0) = 0;

	/**
		<PRE>	
		����˵����	void Flush();
		�������ܣ�	������������д���ļ��У�֮ǰ���ж��Ƿ����㹻Ӳ�̿ռ䣩
		����˵����	��
		����ֵ��	��
		��д�ˣ�	����&��Ԩ&��ѧ��
		������ڣ�
		</PRE>	
	*/
	virtual void Flush() = 0;

	/**
		<PRE>	
		����˵����	void FlushList();
		�������ܣ�	��ʹ�û���ڵ�ʱ������������д���ļ���
					�����ж��Ƿ����㹻Ӳ�̿ռ䣩
		����˵����	��
		����ֵ��	��
		��д�ˣ�	����&��Ԩ
		������ڣ�
		</PRE>	
	*/
	virtual void FlushList() = 0;
	
	/**
		<PRE>	
		����˵����	void FlushAll()
		�������ܣ�	д��ȫ��������Ϣ
		����˵����	��
		����ֵ��	��
		��д�ˣ�	����&��Ԩ
		������ڣ�
		</PRE>	
	*/
	virtual void FlushAll() = 0;

	/**
		<PRE>	
		����˵����	void Destroy();
		�������ܣ�	ʵ���ͷ��ڴ溯��
		����˵����	��
		����ֵ��	��
		��д�ˣ�	����&��Ԩ
		������ڣ�
		</PRE>	
	*/
	virtual void Destroy() = 0;

	/**
		<PRE>	
		����˵����	void FlushTemp()
		�������ܣ�	����ʱ�ڴ��д����־�ļ�
		����˵����	��
		����ֵ��	��
		��д�ˣ�	����&��Ԩ
		������ڣ�
		</PRE>	
	*/
	virtual void FlushTemp() = 0;

	/**
		<PRE>	
		����˵����	void ClearTemp()
		�������ܣ�	�����ʱ�ڴ�����
		����˵����	��
		����ֵ��	��
		��д�ˣ�	��ѧ��
		������ڣ�
		</PRE>	
	*/
	virtual void ClearTemp() = 0;
};

#endif
