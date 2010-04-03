

/************************************
  REVISION LOG ENTRY
  Revision By: Weihua Ju
  Revised on 2003-5-8 17:48:56
  Comments:
  CRequest::CRequest(CRequest& req) has a bug:
  the content of parameter is not be copyed, cause crash when delete the source and cloned request
 ************************************/


/************************************
  REVISION LOG ENTRY
  Revision By: Weihua Ju
  Revised on 2003-3-28 16:44:51
  Comments: 
  modify GenerateRequest()
  old version not set m_lTotalSize = 0 before generate request
 ************************************/


/************************************
  REVISION LOG ENTRY
  Revision By: Weihua Ju
  Revised on 2003-3-10 21:28:57
  Comments: 
  1. Add one constructor which copy content from anther instance
 ************************************/

#pragma warning (disable:4786)
#include "stdio.h"
#include <string>
#include <valarray>
#include <stack>
#include <list>
#include <deque>

#include "os/os.h"

#include "COMMONDEF.H"
//#include "Winsock2.h"
//#include "windows.h"
#include "PubFuncTable.h"
#include "Exp.h"
//#include "DOCommon.h"

#include "compiler.h"
#include "datatype.h"
//#include "ScriptFuncTable.h"
//#include "Function.h"
//#include "VirtualMachine.h"
//#include "VMManager.h"
#include "Exp.h"
#include "Request.h"
#include "se_interface.h"

//extern void (* g_pLogFunc)(char* sMsg, char* sFile, long nLine, long nLevel);		// 日志记录函数指针
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*extern "C" 
{
    unsigned long htonl(unsigned long);
    unsigned short htons(unsigned short);
    unsigned long ntohl(unsigned long);
    unsigned short ntohs(unsigned short);
}*/

void CRequest::Reset()
{
	//cleanup
	if (m_pRequestData)
	{
		delete m_pRequestData;
		m_pRequestData = NULL;
	}

	if (m_pParamList)
	{
		int i;
		for (i = 0; i < m_lParamNum; i++)
		{
			if (m_pParamList[i].content != NULL)
			{
				delete m_pParamList[i].content;
				m_pParamList[i].content = NULL;
			}
		}
		delete m_pParamList;
		m_pParamList = NULL;
	}

	//initialize
	m_lParamNum = 0;
	m_lTotalSize = 0;
	m_pParamList = NULL;
	m_lParamNum = m_lParamListSize = 0;
	m_lIncreaseSize = 5;
	m_pRequestData = NULL;
	m_szFuncID[0] = 0;
	m_szErrMsg[0] = 0;

}
CRequest::CRequest()
{
	m_lTotalSize = 0;
	m_pParamList = NULL;
	m_lParamNum = m_lParamListSize = 0;
	m_lIncreaseSize = 5;
	m_pRequestData = NULL;
	m_szFuncID[0] = 0;
	m_szErrMsg[0] = 0;

}

CRequest::CRequest(CRequest& req)
{
	m_lTotalSize = req.GetTotalSize();

	m_lParamNum = req.GetParamNum();

	m_lParamListSize = req.GetListSize();

	if (m_lParamListSize > 0)
	{
		m_pParamList = new REQPARAM[m_lParamListSize];
		if (m_pParamList == NULL)
			throw new CExp(CExp::err_allocate_memory, "new buffer for param list failed");
		if (m_lParamNum > 0)
		{
			memcpy(m_pParamList, req.GetParam(0), sizeof(REQPARAM)*m_lParamNum);
			int i;
			for (i = 0; i < m_lParamNum; i++)
			{
				// copy content, set pCurrent
				long lOffset = m_pParamList[i].pCurrent - m_pParamList[i].content;
				unsigned char* copy = new unsigned char[m_pParamList[i].bufferSize];
				memcpy(copy, m_pParamList[i].content, m_pParamList[i].contentSize);
				m_pParamList[i].content = copy;
				m_pParamList[i].pCurrent = copy + lOffset;
			}
		}
	}
	
	m_lIncreaseSize = req.GetIncreaseSize();
	m_pRequestData = new unsigned char[req.GetRequestSize()];
	if (m_pRequestData == NULL)
		throw new CExp(CExp::err_allocate_memory, "new buffer for request data failed");
	memcpy(m_pRequestData, req.GetRequest(), req.GetRequestSize());

	strcpy(m_szFuncID, req.GetFuncID());
	strcpy(m_szErrMsg, req.GetErrMsg());

}



CRequest::~CRequest()
{
	Reset();
}

/*
   函数名称     : CRequest::AddParam
   函数功能	    : 手工添加参数
   变量说明     : 
   返回值       : 
   编写人       : 居卫华
   完成日期     : 2001 - 5 - 24
*/
BOOL CRequest::AddParam(char dataType, char arraydim, long arraysize[4], BYTE* content,	long lContentSize, long unitsize)
{
	if (content == NULL)
		return FALSE;
	if (arraydim < 0 || arraydim >4)
		return FALSE;

	//是否有空间
	if (m_lParamNum == m_lParamListSize)
	{
		REQPARAM* pTemp = new REQPARAM[m_lParamNum + m_lIncreaseSize];
		if (pTemp == NULL)
			return FALSE;
		memset(pTemp, 0, sizeof(REQPARAM)*(m_lParamNum + m_lIncreaseSize));
		if (m_pParamList)
		{
			memcpy(pTemp, m_pParamList, sizeof(REQPARAM)*m_lParamNum);
			delete m_pParamList;
		}
		m_pParamList = pTemp;
		m_lParamListSize += m_lIncreaseSize;
	}
	m_pParamList[m_lParamNum].dataType = dataType;
	m_pParamList[m_lParamNum].arraydim = arraydim;
	m_pParamList[m_lParamNum].unitsize = unitsize;

	//set array attribute
	if (arraydim != NULL)
		memcpy(&m_pParamList[m_lParamNum].arraysize, arraysize, arraydim*sizeof(long));

	//set content
	m_pParamList[m_lParamNum].content = new unsigned char[lContentSize];
	if (m_pParamList[m_lParamNum].content == NULL)
		return FALSE;
	m_pParamList[m_lParamNum].bufferSize = lContentSize;
	m_pParamList[m_lParamNum].contentSize = lContentSize;
	memcpy(m_pParamList[m_lParamNum].content, content, lContentSize);
	m_pParamList[m_lParamNum].pCurrent = m_pParamList[m_lParamNum].content+m_pParamList[m_lParamNum].contentSize;
	
	//computer size
	m_lParamNum++;
	return TRUE;
}



/**
函数声明：	__int32 CRequest::GenerateRequest()
函数功能：	生成请求数据块
参数说明：	
返 回 值：	__int32  - 成功返回0， 失败返回非零
编 写 人：	居卫华
完成日期：	2001-7-18
**/
long CRequest::GenerateRequest()
{
	if (m_pRequestData)
	{
		delete m_pRequestData;
		m_pRequestData = NULL;
	}

	m_lTotalSize = 0;
		
	//computer size
	int i,j;
	long idSize = 0;
	long paramsize = 0;
	idSize = strlen(m_szFuncID);
	if (idSize == 0)
	{
		return -1;
	}
	m_lTotalSize += idSize + sizeof(char) + sizeof(long)/*参数数据块长度*/ + sizeof(char)/*parameter number*/;

	for (i = 0; i< this->m_lParamNum; i++)
	{
		paramsize += this->m_pParamList[i].contentSize;
		paramsize += sizeof(short) + sizeof(char) + sizeof(long)+ sizeof(char)+sizeof(long)*4 + sizeof(short);
		//size =     check point   + type         + unitsize    + dim         +arraysize+ checkpoint
		// on win32 paramsize is 26
	}
	m_lTotalSize += paramsize;
	m_pRequestData = new unsigned char[m_lTotalSize];

	unsigned char *p = 	m_pRequestData;
	sprintf((char*)p, m_szFuncID);
	p +=  idSize;
	*p = 0;//end with 0
	p++;
	*(long*)p = paramsize;
	p+=4;
	*(char*)p = (char)m_lParamNum;
	p++; 

	for (i = 0; i< m_lParamNum; i++)
	{
		//0000
		*(short*)p = 0;
		p+=sizeof(short);
		*(char*)p =(char)m_pParamList[i].dataType;
		p++;
		*(long*)p = (long)m_pParamList[i].unitsize;
		p += sizeof(long);
		*(char*)p = (char)m_pParamList[i].arraydim;
		p++;		
		memcpy(p, m_pParamList[i].arraysize, sizeof(long)*4);
		p += sizeof(long)*4;
		//拷贝数据
		switch (m_pParamList[i].dataType)
		{//主机序到网络序
		case dtFloat:
		case dtLong:
		case dtULong:
		case dtInt:
		case dtUInt:
			{
				long count = 1;
				for (j = 0; j < m_pParamList[i].arraydim;j++)
				{
					count *= m_pParamList[i].arraysize[j];
				}
				for (j = 0; j < count; j++)
				{
					((DWORD*)p)[j] = htonl(((DWORD*)(m_pParamList[i].content))[j]);
				}
			}
			break;
		case dtShort:
		case dtUShort:
			{
				long count = 1;
				for (j = 0; j < m_pParamList[i].arraydim;j++)
				{
					count *= m_pParamList[i].arraysize[j];
				}
				for (j = 0; j < count; j++)
				{
					((WORD*)p)[j] = htons(((WORD*)(m_pParamList[i].content))[j]);
				}
			}
			break;
		default:
			memcpy(p, m_pParamList[i].content, m_pParamList[i].contentSize);
		}

		p+= m_pParamList[i].contentSize;
		//ffff
		*(short*)p = -1;
		p += sizeof(short);
/*
				*(char*)p = 0xff;
				p++;
				*(char*)p = 0xff;
				p++;
*/
		
	}
	if (p-m_pRequestData != m_lTotalSize)
	{
		printf("error\r\n");
		//g_pLogFunc("request.GenerateRequest: memory block overflow, this may cause application crash", __FILE__, __LINE__, 1);
		delete m_pRequestData;
		m_pRequestData = NULL;
		return -1;
	}
	return 0;

}

unsigned char* CRequest::GetRequest()
{
	return this->m_pRequestData;
}

long CRequest::GetRequestSize()
{	
	return this->m_lTotalSize;
}

BOOL CRequest::SetFuncID(char *szID)
{
	if (strlen(szID)>20)
		return FALSE;
	sprintf(m_szFuncID, szID);
	return TRUE;
}


/**
函数声明：	BOOL CRequest::AddSimpleParam(char dataType, unsigned char *content, long* index)
函数功能：	添加一个简单参数, 可以是long, float, string, short
参数说明：	
			[IN]char dataType	-	数据类型	
			[IN]unsigned char* content	-	数据值
			[OUT]long* index	-	创建的参数序号
返 回 值：	BOOL  - 成功返回TRUE, 失败反回FALSE
编 写 人：	居卫华
完成日期：	2002-3-15
**/
BOOL CRequest::AddSimpleParam(char dataType, unsigned char *content, long* index)
{
#ifdef __SUPPORT_OBJ
	if (dataType == dtGeneral)
		return FALSE;//结构体应使用AddStructedParam()
#endif
	*index = -1;
	//是否有空间
	if (m_lParamNum == m_lParamListSize)
	{
		REQPARAM* pTemp = new REQPARAM[m_lParamNum + m_lIncreaseSize];
		if (pTemp == NULL)
			return FALSE;
		memset(pTemp, 0, sizeof(REQPARAM)*(m_lParamNum + m_lIncreaseSize));
		if (m_pParamList)
		{
			memcpy(pTemp, m_pParamList, sizeof(REQPARAM)*m_lParamNum);
			delete m_pParamList;
		}
		m_pParamList = pTemp;
		m_lParamListSize += m_lIncreaseSize;
	}
	m_pParamList[m_lParamNum].dataType = dataType;
	m_pParamList[m_lParamNum].arraydim = 0;
	if (dataType == dtString)
		m_pParamList[m_lParamNum].unitsize = strlen((const char*)content)+1;
	else
		m_pParamList[m_lParamNum].unitsize = typesize(dataType);

	//computer size
	if (dataType == dtString)
		m_pParamList[m_lParamNum].contentSize = strlen((const char*)content)+1;
	else
		m_pParamList[m_lParamNum].contentSize = typesize(dataType);
	m_pParamList[m_lParamNum].content = new unsigned char[m_pParamList[m_lParamNum].contentSize];
	if (m_pParamList[m_lParamNum].content == NULL)
		return FALSE;
	m_pParamList[m_lParamNum].pCurrent = m_pParamList[m_lParamNum].content;
	m_pParamList[m_lParamNum].bufferSize = m_pParamList[m_lParamNum].contentSize;
	if ( m_pParamList[m_lParamNum].contentSize > m_pParamList[m_lParamNum].bufferSize)
		return FALSE;
	if (content)
	{
		memcpy(m_pParamList[m_lParamNum].content, content, m_pParamList[m_lParamNum].contentSize);
		m_pParamList[m_lParamNum].pCurrent = m_pParamList[m_lParamNum].content+m_pParamList[m_lParamNum].contentSize;
	}
	*index = m_lParamNum;
	m_lParamNum++;
	return TRUE;
}


BOOL CRequest::InsertSimpleParam(char dataType, unsigned char *content, LONG lIndex)
{
#ifdef __SUPPORT_OBJ
	if (dataType == dtGeneral)
		return FALSE;//结构体应使用AddStructedParam()
#endif
	if (lIndex > m_lParamNum)
		return FALSE;

	//是否有空间
	if (m_lParamNum == m_lParamListSize)
	{
		REQPARAM* pTemp = new REQPARAM[m_lParamNum + m_lIncreaseSize];
		if (pTemp == NULL)
			return FALSE;
		memset(pTemp, 0, sizeof(REQPARAM)*(m_lParamNum + m_lIncreaseSize));
		if (m_pParamList)
		{
			memcpy(pTemp, m_pParamList, sizeof(REQPARAM)*m_lParamNum);
			delete m_pParamList;
		}
		m_pParamList = pTemp;
		m_lParamListSize += m_lIncreaseSize;
	}

	if (lIndex < m_lParamNum)
	{
		REQPARAM* pTemp = new REQPARAM[m_lParamNum - lIndex];
		if (pTemp == NULL)
			return FALSE;
		memcpy((void*)pTemp, &m_pParamList[lIndex], sizeof(REQPARAM)*m_lParamNum - lIndex);
		memcpy((void*)&m_pParamList[lIndex+1], pTemp, sizeof(REQPARAM)*m_lParamNum - lIndex);
		SAFEDELETE(pTemp);
	}
	m_pParamList[lIndex].dataType = dataType;
	m_pParamList[lIndex].arraydim = 0;
	if (dataType == dtString)
		m_pParamList[lIndex].unitsize = strlen((const char*)content)+1;
	else
		m_pParamList[lIndex].unitsize = typesize(dataType);

	//computer size
	if (dataType == dtString)
		m_pParamList[lIndex].contentSize = strlen((const char*)content)+1;
	else
		m_pParamList[lIndex].contentSize = typesize(dataType);
	m_pParamList[lIndex].content = new unsigned char[m_pParamList[lIndex].contentSize];
	if (m_pParamList[lIndex].content == NULL)
		return FALSE;
	m_pParamList[lIndex].pCurrent = m_pParamList[lIndex].content;
	m_pParamList[lIndex].bufferSize = m_pParamList[lIndex].contentSize;
	if ( m_pParamList[lIndex].contentSize > m_pParamList[lIndex].bufferSize)
		return FALSE;
	if (content)
	{
		memcpy(m_pParamList[lIndex].content, content, m_pParamList[lIndex].contentSize);
		m_pParamList[lIndex].pCurrent = m_pParamList[lIndex].content+m_pParamList[lIndex].contentSize;
	}
	m_lParamNum++;
	return TRUE;
}

#if 0
/*
	函数名称     : CRequest::AddArrayParam
	函数功能	    : 添加参数空间, 也可以同时设定内容
	变量说明     : 
			char dataType     - 数据类型
			BYTE *content     - 数据内容, 如果=null则只分配内存不设置内容
			char arraydim     - 数组的维数, 如果=0则不是数组
			long arraysize[]  - 数组各维的大小.如果= NULL , 则不是数组
   返回值       : 
   编写人       : 居卫华
   完成日期     : 2001 - 5 - 24
*/
BOOL CRequest::AddArrayParam(char dataType, long *index, BYTE *content, char arraydim, long arraysize[])
{
	if (arraydim < 0 || arraydim >4)
		return FALSE;
	if (dataType == dtGeneral)
		return FALSE;//结构数组应使用CreateParam()和SetArrayContent();

	*index = -1;
	//是否有空间
	if (m_lParamNum == m_lParamListSize)
	{
		REQPARAM* pTemp = new REQPARAM[m_lParamNum + m_lIncreaseSize];
		if (pTemp == NULL)
			return FALSE;
		memset(pTemp, 0, sizeof(REQPARAM)*(m_lParamNum + m_lIncreaseSize));
		if (m_pParamList)
		{
			memcpy(pTemp, m_pParamList, sizeof(REQPARAM)*m_lParamNum);
			delete m_pParamList;
		}
		m_pParamList = pTemp;
		m_lParamListSize += m_lIncreaseSize;
	}
	m_pParamList[m_lParamNum].dataType = dataType;
	m_pParamList[m_lParamNum].arraydim = arraydim;
	if (dataType == dtString)
		m_pParamList[m_lParamNum].unitsize = strlen((const char*)content)+1;
	else
		m_pParamList[m_lParamNum].unitsize = typesize(dataType);

	if (arraydim != NULL)
		memcpy(&m_pParamList[m_lParamNum].arraysize, arraysize, arraydim*sizeof(long));

	if (arraydim == 0)
	{//非数组参数, 直接使用
		//computer size
		if (dataType == dtString)
			m_pParamList[m_lParamNum].contentSize = strlen((const char*)content)+1;
		else
			m_pParamList[m_lParamNum].contentSize = typesize(dataType);
		m_pParamList[m_lParamNum].content = new unsigned char[m_pParamList[m_lParamNum].contentSize];
		if (m_pParamList[m_lParamNum].content == NULL)
			return FALSE;
		m_pParamList[m_lParamNum].pCurrent = m_pParamList[m_lParamNum].content;
		m_pParamList[m_lParamNum].bufferSize = m_pParamList[m_lParamNum].contentSize;
		if ( m_pParamList[m_lParamNum].contentSize > m_pParamList[m_lParamNum].bufferSize)
			return FALSE;
		if (content)
		{
			memcpy(m_pParamList[m_lParamNum].content, content, m_pParamList[m_lParamNum].contentSize);
			m_pParamList[m_lParamNum].pCurrent = m_pParamList[m_lParamNum].content+m_pParamList[m_lParamNum].contentSize;
		}
	}
	else if (dataType != dtString)
	{//非字符串数组的数组, 可以直接设置, 也可以只分配内存
		long size = typesize(dataType);
		int i;
		for (i = 0; i< arraydim; i++)
		{
			size *= arraysize[i];
		}
		m_pParamList[m_lParamNum].contentSize = size;
		m_pParamList[m_lParamNum].content = new unsigned char[size];
		if (m_pParamList[m_lParamNum].content == NULL)
			return FALSE;
		m_pParamList[m_lParamNum].pCurrent = m_pParamList[m_lParamNum].content;
		m_pParamList[m_lParamNum].bufferSize = m_pParamList[m_lParamNum].contentSize;
		if (content != NULL)
		{
			memcpy(m_pParamList[m_lParamNum].content, content, size);
			m_pParamList[m_lParamNum].pCurrent = m_pParamList[m_lParamNum].content+size;
		}
	}
	else if (content != NULL)
	{//子符串数组, 且内容不为空
		SetArrayContent(m_lParamNum, dtString, content, NULL, 0);
	}
	*index = m_lParamNum;
	m_lParamNum++;
	return TRUE;
}
#endif


/*
   函数名称     : CRequest::SetArrayContent
   函数功能	    : 设定数组的内容
   变量说明     : 
			long lIndex: 参数索引
			char type:   参数类型
			unsigned char *content: 参数内容
			long array[]: 参数在数组中的位置, 如果=NULL, 添加在末尾
			long lStructSize: 结构的大小， 默认为零， 当dataType为dtGeneral是有效
   返回值       : 成功返回TRUE， 失败返回FALSE
   编写人       : 居卫华
   完成日期     : 2001 - 5 - 24
*/
BOOL CRequest::SetArrayContent(long lIndex, char type, unsigned char *content, long array[], long lStructSize)
{
	if (content == NULL|| lIndex >= m_lParamNum || lIndex < 0)
	{
		sprintf(m_szErrMsg, "Invalid param");
		return FALSE;
	}
	if (type<dtFirstType || type > dtLastType)
	{
		sprintf(m_szErrMsg, "Invalid param");
		return FALSE;
	}
	if (m_pParamList[lIndex].dataType != type)
	{
		sprintf(m_szErrMsg, "the input type is not the as same as the type of param");
		return FALSE;
	}

	if (m_pParamList[lIndex].arraydim <= 0)
	{
		sprintf(m_szErrMsg, "this parameter is not an array");
		return FALSE;
	}

	long size = 0;
	if (type == dtString)
	{
		size = strlen((const char*)content)+1;
		if (m_pParamList[lIndex].contentSize + size > m_pParamList[lIndex].bufferSize)
		{//增加空间
			unsigned char* temp = new unsigned char[m_pParamList[lIndex].bufferSize + size*2];
			if (m_pParamList[lIndex].content)
			{
				memcpy(temp, m_pParamList[lIndex].content, m_pParamList[lIndex].contentSize);
				delete m_pParamList[lIndex].content;
			}
			m_pParamList[lIndex].content = temp;
			m_pParamList[lIndex].bufferSize += size*2;
		}
		memcpy(m_pParamList[lIndex].content+ m_pParamList[lIndex].contentSize, content, size);
	//	memset(m_pParamList[lIndex].content+m_pParamList[lIndex].contentSize+size-1, 0, 1);
		m_pParamList[lIndex].contentSize += size;
		m_pParamList[lIndex].pCurrent = m_pParamList[lIndex].content+m_pParamList[lIndex].contentSize;
	}
#ifdef __SUPPORT_OBJ
	else if (type == dtGeneral)
	{
		//单位长度
		size = lStructSize;
		
		if (array)
		{
			//指定位置
			int i;
			for (i = 0; i< this->m_pParamList[lIndex].arraydim; i++)
			{
				size *= array[i];
			}
			if ( lStructSize + size > m_pParamList[m_lParamNum].bufferSize)
			{
				sprintf(m_szErrMsg, "the param's content buffer is too small when set element");
				return FALSE;
			}

			memcpy(m_pParamList[lIndex].content+size, content, lStructSize);
			m_pParamList[lIndex].pCurrent = m_pParamList[lIndex].content+size+lStructSize;
		}
		else//直接添加在末尾
		{	
			if ( lStructSize + m_pParamList[lIndex].pCurrent - m_pParamList[lIndex].content> m_pParamList[lIndex].bufferSize)
			{
				sprintf(m_szErrMsg, "the param's content buffer is too small when append element");
				return FALSE;
			}
			memcpy(m_pParamList[lIndex].pCurrent, content, lStructSize);
			m_pParamList[lIndex].pCurrent += lStructSize;
		}

	}
#endif
	else
	{			
		//单位长度
		size = typesize(type);
		
		if (array)
		{
			//指定位置
			int i;
			for (i = 0; i< this->m_pParamList[lIndex].arraydim; i++)
			{
				size *= array[i];
			}
			if ( typesize(type) + size > m_pParamList[m_lParamNum].bufferSize)
			{
				sprintf(m_szErrMsg, "the param's content buffer is too small when set element");
				return FALSE;
			}
			
			memcpy(m_pParamList[lIndex].content+size, content, typesize(type));
			m_pParamList[lIndex].pCurrent = m_pParamList[lIndex].content+size+typesize(type);
		}
		else//直接添加在末尾
		{	
			if ( typesize(type) + m_pParamList[lIndex].pCurrent - m_pParamList[lIndex].content> m_pParamList[lIndex].bufferSize)
			{
				sprintf(m_szErrMsg, "the param's content buffer is too small when append element");
				return FALSE;
			}
			memcpy(m_pParamList[lIndex].pCurrent, content, typesize(type));
			m_pParamList[lIndex].pCurrent += typesize(type);
		}

		//m_pParamList[lIndex].content += typesize(type);	
	}
	return TRUE;
}

#ifdef __SUPPORT_OBJ

/**
函数声明：	BOOL CRequest::AddStructedParam(unsigned char *content, long lSize, long* index)
函数功能：	添加结构类型的参数
参数说明：	
			[IN]unsigned char *content	-	参数内容
			[IN]long lSize				-	大小
			[OUT]long* index			-	该参数在参数列表中的索引
返 回 值：	BOOL  - 成功或是败
编 写 人：	居卫华
完成日期：	2001-7-18
**/
BOOL CRequest::AddStructedParam(unsigned char *content, long lSize, long* index)
{
	*index = -1;

	//是否有空间
	if (m_lParamNum == m_lParamListSize)
	{
		REQPARAM* pTemp = new REQPARAM[m_lParamNum + m_lIncreaseSize];
		if (pTemp == NULL)
			return FALSE;
		memset(pTemp, 0, sizeof(REQPARAM)*(m_lParamNum + m_lIncreaseSize));
		if (m_pParamList)
		{
			memcpy(pTemp, m_pParamList, sizeof(REQPARAM)*m_lParamNum);
			delete m_pParamList;
		}
		m_pParamList = pTemp;
		m_lParamListSize += m_lIncreaseSize;
	}

	m_pParamList[m_lParamNum].dataType = dtGeneral;
	m_pParamList[m_lParamNum].arraydim = 0;
	m_pParamList[m_lParamNum].unitsize = lSize;

	//computer size	
	m_pParamList[m_lParamNum].contentSize = lSize;
	m_pParamList[m_lParamNum].content = new unsigned char[m_pParamList[m_lParamNum].contentSize];
	if (m_pParamList[m_lParamNum].content == NULL)
		return FALSE;
	m_pParamList[m_lParamNum].pCurrent = m_pParamList[m_lParamNum].content;
	m_pParamList[m_lParamNum].bufferSize = m_pParamList[m_lParamNum].contentSize;
	if (m_pParamList[m_lParamNum].contentSize > m_pParamList[m_lParamNum].bufferSize)
		return FALSE;
	if (content)
	{
		memcpy(m_pParamList[m_lParamNum].content, content, m_pParamList[m_lParamNum].contentSize);
		m_pParamList[m_lParamNum].pCurrent = m_pParamList[m_lParamNum].content+m_pParamList[m_lParamNum].contentSize;
	}
	*index = m_lParamNum;
	m_lParamNum++;
	return TRUE;
}
#endif



/**
函数声明：	BOOL CRequest::CreateParam(char dataType, long* index, long lStructSize, char arraydim, long arraysize[4])
函数功能：	建立一个空的参数
参数说明：	
			[IN]char dataType	-	数据类型
			[OUT]long* index	-	该参数在参数列表中的索引
			[IN]long lStructSize	-	结构类型的大小
			[IN]char arraydim	-	数组的维数
			[IN]long arraysize[4]	-	数组每一维的长度
返 回 值：	BOOL  - 
编 写 人：	居卫华
完成日期：	2001-7-18
**/
BOOL CRequest::CreateParam(char dataType, long* index, long lStructSize, char arraydim, long arraysize[4])
{
	*index = -1;

	if (arraydim < 0 || arraydim >4)
		return FALSE;

	//是否有空间
	if (m_lParamNum == m_lParamListSize)
	{
		REQPARAM* pTemp = new REQPARAM[m_lParamNum + m_lIncreaseSize];
		if (pTemp == NULL)
			return FALSE;
		memset(pTemp, 0, sizeof(REQPARAM)*(m_lParamNum + m_lIncreaseSize));
		if (m_pParamList)
		{
			memcpy(pTemp, m_pParamList, sizeof(REQPARAM)*m_lParamNum);
			delete m_pParamList;
		}
		m_pParamList = pTemp;
		m_lParamListSize += m_lIncreaseSize;
	}
	m_pParamList[m_lParamNum].dataType = dataType;
	m_pParamList[m_lParamNum].arraydim = arraydim;
	if (dataType == dtString)
		m_pParamList[m_lParamNum].unitsize = 0;
#ifdef __SUPPORT_OBJ
	else if (dataType == dtGeneral)
		m_pParamList[m_lParamNum].unitsize = lStructSize;
#endif
	else
		m_pParamList[m_lParamNum].unitsize = typesize(dataType);
	
	if (arraydim != NULL)
		memcpy(&m_pParamList[m_lParamNum].arraysize, arraysize, arraydim*sizeof(long));

	if (dataType != dtString)
	{//非字符串可以预先分配内存

		//数组元素的长度
		long size;
#ifdef __SUPPORT_OBJ
		if (dataType == dtGeneral)
			size = lStructSize;
		else
#endif
			size = typesize(dataType);
		
		int i;
		if (arraydim != 0)
		{
			for (i = 0; i< arraydim; i++)
			{
				size *= arraysize[i];		
			}
		}
		//内容长度
		m_pParamList[m_lParamNum].contentSize = size;
		//分配内存
		m_pParamList[m_lParamNum].content = new unsigned char[size];
		if (m_pParamList[m_lParamNum].content == NULL)
			return FALSE;
		//current 指针
		m_pParamList[m_lParamNum].pCurrent = m_pParamList[m_lParamNum].content;
		//buffer的实际大小
		m_pParamList[m_lParamNum].bufferSize = m_pParamList[m_lParamNum].contentSize;
	}
	*index = m_lParamNum;
	m_lParamNum++;
	return TRUE;
}


/*
函数声明：	long LoadRequest(BYTE* pRequest)
函数功能：	从数据块分析出请求的内容
参数说明：	
			[IN]BYTE* pRequest	-	数据块指针
返 回 值：	long  - 
编 写 人：	居卫华
完成日期：	2001-6-21
*/
long CRequest::LoadRequest(BYTE* pRequest)
{
	int ret = 0;
	int j;
	BYTE* point;	
	
	int  iParamBlkSize = 0;
	int index = 0;
	char paramNum  = 0;
	BYTE* pReadyParam = NULL;//由参数数组合并成的最后的参数块
	int   paramsize = 0;         //参数块的最后长度
	BYTE* pBlock = NULL;

	//reset this	
	Reset();
	
	//local initialize
	point = pRequest;

	//get function name
	strcpy(m_szFuncID, (const char*)point);
	if (strlen(m_szFuncID) <= 0)
	{
		ret = REQERR_NOFUNCNAME;
		goto CleanUp;
	}	 
	point += strlen(m_szFuncID)+sizeof(char);

#ifdef _DEBUG
//	LOG("CRequest: Get Function Name success", 200);
#endif

	//validate pRequest
	iParamBlkSize = *(long*)point;
	point+= sizeof(long);
	if (iParamBlkSize < 0)
	{
		ret = REQERR_SIZELESSZERO;
		goto CleanUp;
	}
	paramNum = *(char*)point;
	point+= sizeof(char);
	if (paramNum < 0)
	{
		ret = REQERR_PARAMNUMERROR;
		goto CleanUp;
	}
	this->m_pParamList = new REQPARAM[paramNum];
	if (m_pParamList == NULL)
	{
		ret = MEMERR_ALLOCFAILED;
		goto CleanUp;
	}
	this->m_lParamListSize = this->m_lParamNum = paramNum;

#ifdef _DEBUG
//	LOG("validate request success", 200)
#endif
	
	//analyze request according to param descript of function
	int i;
	short checkpoint;
	for (i = 0; i < paramNum; i++)
	{
		//validate
		checkpoint = *(short*)point;
		point+= sizeof(short);
		if (checkpoint != 0)
		{
			ret = REQERR_CHECKPOINTERROR;
			goto CleanUp;
		}
		//data type
		m_pParamList[i].dataType = *(char*)point;
		if (m_pParamList[i].dataType < dtFirstType || m_pParamList[i].dataType > dtLastType)
		{
			ret = REQERR_INVALIDDATATYPE;
			goto CleanUp;
		}
		point += sizeof(char);
		//unitsize
		m_pParamList[i].unitsize = *(long*)point;
		if (m_pParamList[i].unitsize < 0)
		{
			ret = REQERR_UNITSIZEERROR;
			goto CleanUp;
		}
		point += sizeof(long);

		//dim
		m_pParamList[i].arraydim = *(char*)point;
		point += sizeof(char);

		//arraysize
		memcpy(m_pParamList[i].arraysize, point, sizeof(long)*4);
		point += sizeof(long)*4;

		//content
		m_pParamList[i].content = point;

		if (m_pParamList[i].dataType == dtString)
		{//如果是字符串 
			int strnum = 1;
			for (j = 0; j < m_pParamList[i].arraydim; j++)
			{
				strnum *= m_pParamList[i].arraysize[j];				
			}		
			for (j = 0; j<strnum; j++)
			{
				point += strlen((const char*)point)+sizeof(char);
			}		
			m_pParamList[i].contentSize = point - m_pParamList[i].content;
		}
#ifdef __SUPPORT_OBJ
		else if (m_pParamList[i].dataType == dtGeneral)
		{
			m_pParamList[i].contentSize = m_pParamList[i].unitsize;
			for (j = 0; j < m_pParamList[i].arraydim; j++)
			{
				m_pParamList[i].contentSize *= m_pParamList[i].arraysize[j];
			}
			point += m_pParamList[i].contentSize;	
		}
#endif
		else
		{
		//如果不是字符串
			m_pParamList[i].contentSize = typesize(m_pParamList[i].dataType);
			for (j = 0; j < m_pParamList[i].arraydim; j++)
			{
				m_pParamList[i].contentSize *= m_pParamList[i].arraysize[j];
			}
			point += m_pParamList[i].contentSize;	
		}
		//validate
		checkpoint = *(short*)point;

		if (checkpoint != -1)
		{
			ret = REQERR_CHECKPOINTERROR;
			goto CleanUp;
		}

		//copy data
		long size = point - m_pParamList[i].content;
		BYTE* pstart = m_pParamList[i].content;
		m_pParamList[i].content = new unsigned char[size];
		if (m_pParamList[i].content == NULL)
		{
			ret = MEMERR_ALLOCFAILED;
			goto CleanUp;
		}
		m_pParamList[i].bufferSize = size;
		switch (m_pParamList[i].dataType)
		{
		case dtFloat:
		case dtLong:
		case dtULong:
		case dtInt:
		case dtUInt:
			{
				long count = 1;
				for (j = 0; j < m_pParamList[i].arraydim;j++)
				{
					count *= m_pParamList[i].arraysize[j];
				}
				if (count*sizeof(DWORD) > size)
				{
					printf("error\r\n");
					//g_pLogFunc("request.LoadRequest: memory block overflow, this may cause application crash", __FILE__, __LINE__, 1);
					ret = REQERR_PARAMBLOCKOVERFLOW;
					goto CleanUp;
				}
				for (j = 0; j < count; j++)
				{
					((DWORD*)(m_pParamList[i].content))[j] = ntohl(((DWORD*)pstart)[j]);
				}
			}
		break;
		case dtShort:
		case dtUShort:
			{
				long count = 1;
				for ( j = 0; j < m_pParamList[i].arraydim;j++)
				{
					count *= m_pParamList[i].arraysize[j];
				}
				if (count*sizeof(WORD) > size)
				{
					printf("error\r\n");
//					g_pLogFunc("request.LoadRequest: memory block overflow, this may cause application crash", __FILE__, __LINE__, 1);
					ret = REQERR_PARAMBLOCKOVERFLOW;
					goto CleanUp;
				}
				for (j = 0; j < count; j++)
				{
					((WORD*)(m_pParamList[i].content))[j] = ntohs(((WORD*)pstart)[j]);
				}
			}
			break;
		default:
			memcpy(m_pParamList[i].content, pstart, size);
		}
		point += sizeof(short);
	}

CleanUp:
#ifdef _DEBUG
//	LOG("CRequest: CleanUp of LoadRequest()", 200)
#endif
	//destroy virtal machine and release function
	if (pReadyParam) 
		delete pReadyParam;
	return ret;
}

long CRequest::typesize(char cType)
{
	switch (cType)
	{
	case dtChar:
	case dtUChar:
		return 1;break;
	case dtShort:
	case dtUShort:
		return 2;break;
	case dtInt:
	case dtUInt:		
	case dtLong:
	case dtULong:
	case dtFloat:
		return 4;
		break;
	default: 
		return -1;
	}
}


/**
函数声明：	char* CRequest::GetErrMsg()
函数功能：	取错误信息
参数说明：	
返 回 值：	char*  - 错误信息
编 写 人：	居卫华
完成日期：	2001-7-18
**/
char* CRequest::GetErrMsg()
{
	return m_szErrMsg;
}
