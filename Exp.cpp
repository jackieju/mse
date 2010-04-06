#include "os/os.h"
#include "Exp.h"

// error code
const long CExp::ret_ok							= 0;
const long CExp::err_unknown					= err_basic_base + 1;
const long CExp::err_invalid_param				= err_basic_base + 2;
const long CExp::err_allocate_memory			= err_basic_base + 3;
const long CExp::err_unexpected_status			= err_basic_base + 4;
const long CExp::err_createdbconnectpool_failed = err_basic_base + 5;
const long CExp::err_getdbstmt_failed			= err_basic_base + 6;
const long CExp::err_dbop_failed				= err_basic_base + 7;
const long CExp::err_createevent_failed			= err_basic_base + 8;

//const long CExp::err_user_defined				= 100;

// status code
const long CExp::stat_null				= -1;
const long CExp::stat_uninitialized		= 0;
const long CExp::stat_init_ok			= 2;
const long CExp::stat_starting_no_err	= 3;
const long CExp::stat_starting_with_err	= 4;
const long CExp::stat_stoping			= 5;
const long CExp::stat_stopped_ok		= 6;
const long CExp::stat_stopped_with_err	= 7;
const long CExp::stat_user_defined		= 100;


/**
函数声明：	CExp::CExp(long lCode, char* sMsg, char* file, long nLine,  long lState)
函数功能：	constructor
参数说明：	
			[IN]long lCode	-	异常代码
			[IN]char* sMsg	-	异常信息(optional)
			[IN]char* file	-	发生异常的source文件(optional)
			[IN]long nLine	-	发生异常的所在行号(optional)
			[IN]long lState	-	状态代码, 用于描述发生异常时的系统状态(optional)
返 回 值：	 - 
编 写 人：	居卫华
完成日期：	2003-4-23
**/
CExp::CExp(long lCode, char* sMsg, char* file, long nLine,  long lState, long lLevel)
{
	pLow = NULL;
	m_lState = stat_null;
	m_nLine = -1;
	m_sFile[0] = 0;
	memset(m_sMsg, 0, MSGSIZE);

	if (sMsg)
	{
		snprintf(m_sMsg, MSGSIZE-1, "%s", sMsg);
	}

	if (file)
		strcpy(m_sFile, file);

	if (nLine > 0)
		m_nLine = nLine;

	m_lExpCode = lCode;

	m_lLevel = lLevel;
}

CExp::~CExp() throw ()
{
	SAFEDELETE(pLow);
}


/**
函数声明：	char* Show()
函数功能：	output all mag to string
参数说明：	
返 回 值：	char*  - String contain the exception content, 
			the string should be deleted after use, otherwise resource will be leaked.
编 写 人：	居卫华
完成日期：	2003-4-23
**/
char* CExp::Show()
{
	char* sLowOutput = NULL;	// lower exception message
	long lSize = 4096;			// size of buffer to be put message
	
	if (pLow)
	{
		// get lower exception content and size
		sLowOutput = pLow->Show();
		lSize += strlen(sLowOutput);
	}
	
	char* sOutput = new char[lSize];	// allocate buffer for all message include lower exception's content
	
	if (pLow)	// has lower exception
		snprintf(sOutput, lSize-1, "Exception(line %d @ file %s):code=%d, state=%d, msg='%s'\n%s", m_nLine, m_sFile, m_lExpCode, m_lState, m_sMsg, sLowOutput);
	else		// has no lower exception
		snprintf(sOutput, lSize-1, "Exception(line %d @ file %s):code=%d, state=%d, msg='%s'\n", m_nLine, m_sFile, m_lExpCode, m_lState, m_sMsg);
	
	SAFEDELETE(sLowOutput);
	
	return sOutput;
}