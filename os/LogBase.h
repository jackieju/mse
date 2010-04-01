#ifndef __COMMONLIB_LOG__
#define __COMMONLIB_LOG__

//		日志保存频率定义	
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
	对象名称：	 日志记录类
	对象描述：   将字符串写入日志文件
	重要函数：   
	编写人：     刘骏&张渊&陈学光
	完成日期：   
	</PRE>	
*/
class CLogBase 
{
public:
	/**
		<PRE>	
		函数说明：	void SetBaseErrorLevel(long nNewErrorLevel);
		函数功能：	设置日志记录错误缓冲级别
		变量说明：	nNewErrorLevel  新的日志错误缓冲级别
					大于此级别的日志信息将不再记录(一般>=0)			
		返回值：	无
		编写人：	陈学光
		完成日期：
		</PRE>	
	*/
	virtual void SetBaseErrorLevel(long nNewErrorLevel) = 0;

	/**
		<PRE>	
		函数说明：	void SetMaxCacheNum(int nMaxCacheNum);
		函数功能：	设置最大日志节点
		变量说明：	nMaxCacheNum 新的最大日志节点
					此值必须>0 如<= 0，写入5
		返回值：	无
		编写人：	刘骏&张渊
		完成日期：
		</PRE>	
	*/
	virtual void SetMaxCacheNum(int nMaxCacheNum = 5) = 0;

	/**
		<PRE>	
		函数说明：	void SetMaxFreeDisk(int nMaxFreeDisk);
		函数功能：	设置最大剩余空间(以M为单位)，如硬盘剩余空间小于此值时
					不再写入日志，仅于m_bCacheMode 为True时有效
		变量说明：	m_nMaxFreeDisk 新的最大剩余空间
		返回值：	无
		编写人：	刘骏&张渊
		完成日期：
		</PRE>	
	*/
	virtual void SetMaxFreeDisk(int nMaxFreeDisk) = 0;


	/**
		<PRE>	
		函数说明：	long FindSeparatorNumber(char *sBuffer);
		函数功能：	找到指定字符串中分隔符的个数
		变量说明：	sBuffer 欲查找的内容
		返回值：	无
		编写人：	刘骏&张渊
		完成日期：
		</PRE>
	*/
	virtual long FindSeparatorNumber(char *sBuffer) = 0;

	/**
		<PRE>	
		函数说明：	void GenerateFileName(char* sLogFileName,long tTime);
		函数功能：	生成新文件名
		变量说明：	tTime			写入的日期及时间       
		返回值：	sLogFileName	返回的生成结果文件名
		编写人：	刘骏&张渊
		完成日期：
		</PRE>	
	*/
	virtual void GenerateFileName(char* sLogFileName,long tTime) = 0;

	/**
		<PRE>	
		函数说明：	long InitLog(...)
		函数功能：	打开普通文本方式存储的Log文件
		变量说明：	sLogFileName	文件名(不允许为空)
					sSysName		文件头标识系统名 为空写入[Unknown System]
					sVersion        文件头标识版本名 为空写入[Unknown Version]
					nFieldNum		字段数，必须>=1
					sFieldName		对应字段的字段名,名字间由nSeparator分隔
									如与字段数不能匹配，则返回错误
					nBufferSize     缓存区大小  必须>0
					bCacheMode		是否缓存写入
					nSaveMode		按何方式显示生成新文件
					nSeparator		分隔符
					nErrorLevel		写入日志等级
					nRepeatNum		写入日志的重复记录数
					nMaxCacheNum	最大日志节点数
		返回值：	0						  成功
					errInvalideParameter      参数错误
					errAllocateMemory         内存分配错误
		编写人：	刘骏&张渊
		完成日期：
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
		函数说明：	void Put(char *pMsg,long nErrorLevel = 0);
		函数功能：	写入文本日志内容
		变量说明：	pMsg            日志字符串
					nErrorLevel     日志等级记录,如大于m_nBaseErrorLevel
									则不写入日志
		返回值：	无
		编写人：	刘骏&张渊&陈学光
		完成日期：
		</PRE>	
	*/
	virtual void Put(char *pMsg,long nErrorLevel = 0) = 0;

	/**
		<PRE>	
		函数说明：	void Put(char *pMsg,long nErrorLevel = 0);
		函数功能：	写入文本日志内容，增加行号选项
		变量说明：	pMsg            日志字符串
					pcLogFileName   产生日志的源文件名
					lLogPosition	产生日志的行号
					nErrorLevel     日志等级记录,如大于m_nBaseErrorLevel
									则不写入日志
		返回值：	无
		编写人：	刘骏&张渊&陈学光
		完成日期：
		</PRE>	
	*/
	virtual void Put(char *pMsg,char *pcLogFileName,long lLogPosition,long nErrorLevel = 0) = 0;

	/**
		<PRE>	
		函数说明：	void Flush();
		函数功能：	将缓冲区内容写入文件中（之前先判断是否有足够硬盘空间）
		变量说明：	无
		返回值：	无
		编写人：	刘骏&张渊&陈学光
		完成日期：
		</PRE>	
	*/
	virtual void Flush() = 0;

	/**
		<PRE>	
		函数说明：	void FlushList();
		函数功能：	在使用缓冲节点时将缓冲区内容写入文件中
					（不判断是否有足够硬盘空间）
		变量说明：	无
		返回值：	无
		编写人：	刘骏&张渊
		完成日期：
		</PRE>	
	*/
	virtual void FlushList() = 0;
	
	/**
		<PRE>	
		函数说明：	void FlushAll()
		函数功能：	写入全部缓冲信息
		变量说明：	无
		返回值：	无
		编写人：	刘骏&张渊
		完成日期：
		</PRE>	
	*/
	virtual void FlushAll() = 0;

	/**
		<PRE>	
		函数说明：	void Destroy();
		函数功能：	实际释放内存函数
		变量说明：	无
		返回值：	无
		编写人：	刘骏&张渊
		完成日期：
		</PRE>	
	*/
	virtual void Destroy() = 0;

	/**
		<PRE>	
		函数说明：	void FlushTemp()
		函数功能：	将临时内存块写入日志文件
		变量说明：	无
		返回值：	无
		编写人：	刘骏&张渊
		完成日期：
		</PRE>	
	*/
	virtual void FlushTemp() = 0;

	/**
		<PRE>	
		函数说明：	void ClearTemp()
		函数功能：	清除临时内存内容
		变量说明：	无
		返回值：	无
		编写人：	陈学光
		完成日期：
		</PRE>	
	*/
	virtual void ClearTemp() = 0;
};

#endif
