

/************************************
  REVISION LOG ENTRY
  Revision By: Weihua Ju
  Revised on 2003-3-12 20:46:17
  Comments: 
  Add one interface "InsertParam" to CRequest

 ************************************/


/************************************
  REVISION LOG ENTRY
  Revision By: Weihua Ju
  Revised on 2003-3-10 21:28:57
  Comments: 
  1. Add one constructor which copy content from anther instance
 ************************************/


#if !defined(AFX_REQUEST_H__BB9CC8CC_9A22_4FE1_BA9E_2509FA2BD9AA__INCLUDED_)
#define AFX_REQUEST_H__BB9CC8CC_9A22_4FE1_BA9E_2509FA2BD9AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct _tagREQPARAM
{
    char dataType;       //参数的类型
	long unitsize;    //数据的单位长度
	char arraydim;       //数组维数(最大四维)
	long  arraysize[4];   //数组长度
	unsigned char* content;       //内容	
	long   contentSize;   //内容的长度
	long   bufferSize;    //content内存块的大小
	unsigned char* pCurrent;
}REQPARAM;

//添加简单数据
#define PARAM_LONG(req, data) req.AddParam(dtLong, 0, 0, data, sizeof(long), sizeof(dtLong))
#define PARAM_INT(req, data)  req.AddParam(dtInt, 0, 0, data, sizeof(long), sizeof(long))
#define PARAM_CHAR(req, data) req.AddParam(dtChar, 0, 0, data, sizeof(dtChar), sizeof(dtChar))
#define PARAM_SHORT(req, data) req.AddParam(dtShort, 0, 0, data, sizeof(dtShort), sizeof(dtShort))
#define PARAM_STRING(req, data) req.AddParam(dtString, 0, 0, data, strlen(data), strlen(data))
#define PARAM_STRUCT(req, data, size ) req.AddParam(dtObject, 0, 0, data, size, size)
/*添加一个复杂类型的数组
1. 	CreateParam(dataType, pindex, lEleSize, arraydim, arraysize[4]);
2.  SetArrayParam()如果不指定位置则添加在末尾
*/
/*添加一个连续的数组
AddParam(类型， dim， dimarray， data， size)

*/

/**
   对象名称     : CRequest
   对象描述	    : 请求类, 封装了请求的生成和解析等操作
   重要函数     :
	//reset
	void Reset();
－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－
－－	创建请求的的接口
－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－
	//添加一个简单参数, 可以是long, float, string, short
	BOOL AddSimpleParam(char dataType, unsigned char* content, long* index);
	//设置参数(数组或非数组）的值
	BOOL SetArrayContent(long lIndex, char type, unsigned char *content, long array[] = NULL, long lStructSize = 0); 
	//建立一个空的参数
	BOOL CreateParam(char dataType, long* index, long lStructSize = 0, char arraydim = 0, long arraysize[4] = NULL);
	//从数据块分析出请求的内容
	long LoadRequest(BYTE* pRequest);
	//添加一个结构数据
	BOOL AddStructedParam(unsigned char *content, long lSize, long* index);	
	//设置请求的脚本函数
	void SetFuncID(char* szID);
	//取请求的大小
	long GetRequestSize();
	//取请求数据块
	unsigned char* GetRequest();
	//生成请求数据块
	long GenerateRequest();
	//添加参数
	BOOL AddParam(
	char dataType,       //参数的类型	
	char arraydim,       //数组维数(最大四维)
	long  arraysize[4],   //数组长度
	BYTE* content,       //内容	
	long lContentSize
	, long unitsize);
－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－
－－	获取请求信息的接口
－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－
	//取错误描述
	char* GetErrMsg();
	//取参数个数
	long GetParamNum();
	//取参数列表
	REQPARAM* GetParamList();
	//取请求的脚本函数ID
	char* GetFuncID();
	//计算简单数据类型的大小, 不能计算结构类型的大小
	long typesize(char cType);
 
 
   编写人       : 居卫华
   完成日期     : 2001 - 7 - 18

－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－
－－	使用指南
－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－
	1. 添加long型参数	long param
	long index;	// 参数序号
	if (!m_request->AddSimpleParam(dtLong, (unsigned char*)&param, &index))
	{
		sprintf(m_szErrMsg, "add Long parameter <%ld> failed", param);
		LOG50(m_szErrMsg);
		throw new CPSClientExp(m_szErrMsg, RC_REQOP, __FILE__, __LINE__);
		return RC_REQOP;
	}

	2. 添加string型参数char* param
	CString str = param;
	long index;	// 参数序号
	const char *szString = LPCTSTR(str);
	if (!m_request->AddSimpleParam(dtString, (unsigned char*)szString, &index))
	{
		sprintf(m_szErrMsg, "add string parameter <%s> failed, current parameter number=%d", szString, m_request->GetParamNum());
		LOG200(m_szErrMsg);
		throw new CPSClientExp(m_szErrMsg, RC_REQOP, __FILE__, __LINE__);
		return RC_REQOP;
	}
	3. 添加float型的参数float param
	long index;		// 参数序号
	if (!m_request->AddSimpleParam(dtFloat, (unsigned char*)&param, &index))
	{
		sprintf(m_szErrMsg, "add float parameter <%f> failed", param);
		LOG50(m_szErrMsg);
		throw new CPSClientExp(m_szErrMsg, RC_REQOP, __FILE__, __LINE__);
		return RC_REQOP;
	}

	4. 添加short型参数
	long index;		// 参数序号
	if (!m_request->AddSimpleParam(dtShort, (unsigned char*)&param, &index))
	{
		sprintf(m_szErrMsg, "CPSRequest::AddParamShort: add Short parameter <%d> failed, current parameter number=%d", param, m_request->GetParamNum());
		LOG50(m_szErrMsg);
		throw new CPSClientExp(m_szErrMsg, RC_REQOP, __FILE__, __LINE__);
		return RC_REQOP;
	}
	
	5. 添加数组
	第一步. 添加数组空间(一维数组), 数组大小为long dim, 最大可以使用四维数组
	long index;
	long dimsize[4]; 
	memset(dimsize, 0, sizeof(long)*4);
	dimsize[0] = dim;	// 第一维的长度
	if (!m_request->CreateParam(<元素的数据类型可以是dtLong, dtString, dtShort, dtfloat>, &index, 0(如果是结构数组的话， 结构的大小), 1(数组维数), dimsize(数组描述)))
	{
		LOG50("create param failed, when add dtString array")
		throw new CPSClientExp("create param failed, when add dtSting array", RC_REQOP, __FILE__, __LINE__);
		return RC_REQOP;
	}
	第二步. 添加数组元素float element(假设是float型的数组)
	long index;
	index = m_request->GetParamNum() - 1;
	char sMsg[100];
	if (!m_request->SetArrayContent(index, dtFloat, (unsigned char*)&element))
	{
		sprintf(sMsg, "Set array content failed, when add float element[%f] to long array", element);
		LOG50(sMsg)
		throw new CPSClientExp(sMsg, RC_REQOP, __FILE__, __LINE__);
		return RC_REQOP;
	}

**/
#ifdef WIN32
class __declspec(dllexport)CRequest
#else
class CRequest
#endif
{
public:

	void Reset();
	BOOL SetArrayContent(long lIndex, char type, unsigned char *content, long array[] = NULL, long lStructSize = 0);//设置参数(数组或非数组）的值 
//	BOOL StructContent(long lIndex, char type, unsigned char *content);//设置参数(数组或非数组）的值 

#if 0	
	/*添加参数空间, 也可以同时设定内容
	char dataType     - 数据类型
	BYTE *content     - 数据内容, 如果=null则只分配内存不设置内容
	char arraydim     - 数组的维数, 如果=0则不是数组
	long arraysize[]  - 数组各维的大小.如果= NULL , 则不是数组
	*/
	BOOL AddArrayParam(char dataType, long* index, BYTE *content = NULL, char arraydim = 0, long arraysize[4] = NULL);//添加参数空间, 也可以同时设定内容
#endif

	BOOL CreateParam(char dataType, long* index, long lStructSize = 0, char arraydim = 0, long arraysize[4] = NULL);//建立一个空的参数

	BOOL AddSimpleParam(char dataType, unsigned char* content, long* index);//添加一个简单数据, 可以是long, float, string, short

	long LoadRequest(BYTE* pRequest);//从数据块分析出请求的内容

#ifdef __SUPPORT_OBJ
	BOOL AddStructedParam(unsigned char *content, long lSize, long* index);	//添加一个结构数据
#endif

	BOOL SetFuncID(char* szID);

	long GetRequestSize();

	unsigned char* GetRequest();

	long GenerateRequest();
	BOOL AddParam(
	char dataType,       //参数的类型	
	char arraydim,       //数组维数(最大四维)
	long  arraysize[4],   //数组长度
	BYTE* content,       //内容	
	long lContentSize
	, long unitsize);

	CRequest();

	CRequest(CRequest& req);

	virtual ~CRequest();



	
	long GetParamNum(){return m_lParamNum;};
	REQPARAM* GetParam(long lIndex){if (lIndex >= m_lParamNum || lIndex < 0) return NULL; return &(m_pParamList[lIndex]);} 
	REQPARAM* GetParamList(){return m_pParamList;}
	char* GetFuncID(){return m_szFuncID;}
	long GetListSize(){return m_lParamListSize;
	}
	long GetIncreaseSize(){
		return m_lIncreaseSize;
	}
	char* GetErrMsg();
	long GetTotalSize(){
		return m_lTotalSize;
	}
	BOOL InsertSimpleParam(char dataType, unsigned char *content, LONG lIndex);
		
private:

	REQPARAM* m_pParamList;    //参数表
	long m_lParamListSize;   //参数表长度
	long m_lIncreaseSize;    //列表的增量单位
	long m_lParamNum;        //number of parameter
	char m_szFuncID[21];
	char m_szErrMsg[1024];
	long m_lTotalSize;       //数据块总长
	unsigned char* m_pRequestData;
	
public:
	static long typesize(char cType);
};

#endif // !defined(AFX_REQUEST_H__BB9CC8CC_9A22_4FE1_BA9E_2509FA2BD9AA__INCLUDED_)
