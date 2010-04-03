

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
    char dataType;       //����������
	long unitsize;    //���ݵĵ�λ����
	char arraydim;       //����ά��(�����ά)
	long  arraysize[4];   //���鳤��
	unsigned char* content;       //����	
	long   contentSize;   //���ݵĳ���
	long   bufferSize;    //content�ڴ��Ĵ�С
	unsigned char* pCurrent;
}REQPARAM;

//��Ӽ�����
#define PARAM_LONG(req, data) req.AddParam(dtLong, 0, 0, data, sizeof(long), sizeof(dtLong))
#define PARAM_INT(req, data)  req.AddParam(dtInt, 0, 0, data, sizeof(long), sizeof(long))
#define PARAM_CHAR(req, data) req.AddParam(dtChar, 0, 0, data, sizeof(dtChar), sizeof(dtChar))
#define PARAM_SHORT(req, data) req.AddParam(dtShort, 0, 0, data, sizeof(dtShort), sizeof(dtShort))
#define PARAM_STRING(req, data) req.AddParam(dtString, 0, 0, data, strlen(data), strlen(data))
#define PARAM_STRUCT(req, data, size ) req.AddParam(dtObject, 0, 0, data, size, size)
/*���һ���������͵�����
1. 	CreateParam(dataType, pindex, lEleSize, arraydim, arraysize[4]);
2.  SetArrayParam()�����ָ��λ���������ĩβ
*/
/*���һ������������
AddParam(���ͣ� dim�� dimarray�� data�� size)

*/

/**
   ��������     : CRequest
   ��������	    : ������, ��װ����������ɺͽ����Ȳ���
   ��Ҫ����     :
	//reset
	void Reset();
������������������������������������������������������������������������
����	��������ĵĽӿ�
������������������������������������������������������������������������
	//���һ���򵥲���, ������long, float, string, short
	BOOL AddSimpleParam(char dataType, unsigned char* content, long* index);
	//���ò���(���������飩��ֵ
	BOOL SetArrayContent(long lIndex, char type, unsigned char *content, long array[] = NULL, long lStructSize = 0); 
	//����һ���յĲ���
	BOOL CreateParam(char dataType, long* index, long lStructSize = 0, char arraydim = 0, long arraysize[4] = NULL);
	//�����ݿ���������������
	long LoadRequest(BYTE* pRequest);
	//���һ���ṹ����
	BOOL AddStructedParam(unsigned char *content, long lSize, long* index);	
	//��������Ľű�����
	void SetFuncID(char* szID);
	//ȡ����Ĵ�С
	long GetRequestSize();
	//ȡ�������ݿ�
	unsigned char* GetRequest();
	//�����������ݿ�
	long GenerateRequest();
	//��Ӳ���
	BOOL AddParam(
	char dataType,       //����������	
	char arraydim,       //����ά��(�����ά)
	long  arraysize[4],   //���鳤��
	BYTE* content,       //����	
	long lContentSize
	, long unitsize);
������������������������������������������������������������������������
����	��ȡ������Ϣ�Ľӿ�
������������������������������������������������������������������������
	//ȡ��������
	char* GetErrMsg();
	//ȡ��������
	long GetParamNum();
	//ȡ�����б�
	REQPARAM* GetParamList();
	//ȡ����Ľű�����ID
	char* GetFuncID();
	//������������͵Ĵ�С, ���ܼ���ṹ���͵Ĵ�С
	long typesize(char cType);
 
 
   ��д��       : ������
   �������     : 2001 - 7 - 18

������������������������������������������������������������������������
����	ʹ��ָ��
������������������������������������������������������������������������
	1. ���long�Ͳ���	long param
	long index;	// �������
	if (!m_request->AddSimpleParam(dtLong, (unsigned char*)&param, &index))
	{
		sprintf(m_szErrMsg, "add Long parameter <%ld> failed", param);
		LOG50(m_szErrMsg);
		throw new CPSClientExp(m_szErrMsg, RC_REQOP, __FILE__, __LINE__);
		return RC_REQOP;
	}

	2. ���string�Ͳ���char* param
	CString str = param;
	long index;	// �������
	const char *szString = LPCTSTR(str);
	if (!m_request->AddSimpleParam(dtString, (unsigned char*)szString, &index))
	{
		sprintf(m_szErrMsg, "add string parameter <%s> failed, current parameter number=%d", szString, m_request->GetParamNum());
		LOG200(m_szErrMsg);
		throw new CPSClientExp(m_szErrMsg, RC_REQOP, __FILE__, __LINE__);
		return RC_REQOP;
	}
	3. ���float�͵Ĳ���float param
	long index;		// �������
	if (!m_request->AddSimpleParam(dtFloat, (unsigned char*)&param, &index))
	{
		sprintf(m_szErrMsg, "add float parameter <%f> failed", param);
		LOG50(m_szErrMsg);
		throw new CPSClientExp(m_szErrMsg, RC_REQOP, __FILE__, __LINE__);
		return RC_REQOP;
	}

	4. ���short�Ͳ���
	long index;		// �������
	if (!m_request->AddSimpleParam(dtShort, (unsigned char*)&param, &index))
	{
		sprintf(m_szErrMsg, "CPSRequest::AddParamShort: add Short parameter <%d> failed, current parameter number=%d", param, m_request->GetParamNum());
		LOG50(m_szErrMsg);
		throw new CPSClientExp(m_szErrMsg, RC_REQOP, __FILE__, __LINE__);
		return RC_REQOP;
	}
	
	5. �������
	��һ��. �������ռ�(һά����), �����СΪlong dim, ������ʹ����ά����
	long index;
	long dimsize[4]; 
	memset(dimsize, 0, sizeof(long)*4);
	dimsize[0] = dim;	// ��һά�ĳ���
	if (!m_request->CreateParam(<Ԫ�ص��������Ϳ�����dtLong, dtString, dtShort, dtfloat>, &index, 0(����ǽṹ����Ļ��� �ṹ�Ĵ�С), 1(����ά��), dimsize(��������)))
	{
		LOG50("create param failed, when add dtString array")
		throw new CPSClientExp("create param failed, when add dtSting array", RC_REQOP, __FILE__, __LINE__);
		return RC_REQOP;
	}
	�ڶ���. �������Ԫ��float element(������float�͵�����)
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
	BOOL SetArrayContent(long lIndex, char type, unsigned char *content, long array[] = NULL, long lStructSize = 0);//���ò���(���������飩��ֵ 
//	BOOL StructContent(long lIndex, char type, unsigned char *content);//���ò���(���������飩��ֵ 

#if 0	
	/*��Ӳ����ռ�, Ҳ����ͬʱ�趨����
	char dataType     - ��������
	BYTE *content     - ��������, ���=null��ֻ�����ڴ治��������
	char arraydim     - �����ά��, ���=0��������
	long arraysize[]  - �����ά�Ĵ�С.���= NULL , ��������
	*/
	BOOL AddArrayParam(char dataType, long* index, BYTE *content = NULL, char arraydim = 0, long arraysize[4] = NULL);//��Ӳ����ռ�, Ҳ����ͬʱ�趨����
#endif

	BOOL CreateParam(char dataType, long* index, long lStructSize = 0, char arraydim = 0, long arraysize[4] = NULL);//����һ���յĲ���

	BOOL AddSimpleParam(char dataType, unsigned char* content, long* index);//���һ��������, ������long, float, string, short

	long LoadRequest(BYTE* pRequest);//�����ݿ���������������

#ifdef __SUPPORT_OBJ
	BOOL AddStructedParam(unsigned char *content, long lSize, long* index);	//���һ���ṹ����
#endif

	BOOL SetFuncID(char* szID);

	long GetRequestSize();

	unsigned char* GetRequest();

	long GenerateRequest();
	BOOL AddParam(
	char dataType,       //����������	
	char arraydim,       //����ά��(�����ά)
	long  arraysize[4],   //���鳤��
	BYTE* content,       //����	
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

	REQPARAM* m_pParamList;    //������
	long m_lParamListSize;   //��������
	long m_lIncreaseSize;    //�б��������λ
	long m_lParamNum;        //number of parameter
	char m_szFuncID[21];
	char m_szErrMsg[1024];
	long m_lTotalSize;       //���ݿ��ܳ�
	unsigned char* m_pRequestData;
	
public:
	static long typesize(char cType);
};

#endif // !defined(AFX_REQUEST_H__BB9CC8CC_9A22_4FE1_BA9E_2509FA2BD9AA__INCLUDED_)
